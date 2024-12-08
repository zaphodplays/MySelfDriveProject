#include "LandscapeSplineToSplineComponent.h"

#include "LandscapeSplineActor.h"
#include "LandscapeSplineControlPoint.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Algo/Reverse.h"

DEFINE_LOG_CATEGORY_STATIC(LogSplineConversionActor, Log, All);
// Sets default values
ALandscapeSplineToSplineComponent::ALandscapeSplineToSplineComponent()
{
    // Create the spline component
    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

    // Attach it to the root component (or create a root component if there isn't one)
    RootComponent = SplineComponent;
}

TArray<TObjectPtr<ULandscapeSplineSegment>> ALandscapeSplineToSplineComponent::GetNeighbourSegments(
    const TObjectPtr <ULandscapeSplineSegment> inSegment)
{
    if (!inSegment) return {};

    auto OtherSegmentAtPoint =
        [](TObjectPtr <ULandscapeSplineControlPoint> Point, TObjectPtr <ULandscapeSplineSegment> Segment)
        {
            if (Point->ConnectedSegments.Num() != 2) return TObjectPtr <ULandscapeSplineSegment>();

            TObjectPtr <ULandscapeSplineSegment> out = Point->ConnectedSegments[0].Segment;
            if (out == Segment)
            {
                out = Point->ConnectedSegments[1].Segment;
            }

            return out;
        };

    TArray<TObjectPtr<ULandscapeSplineSegment>> outSplineSegments;
    outSplineSegments.Add(inSegment);

    auto NextSegment = inSegment;
    auto ControlPoint1 = NextSegment->Connections[0].ControlPoint;
    if (!ControlPoint1) return {};
    // iterate forward from the input segment
    // == 2 means that there are exactly two segments in the point, 
    // so the point is part of a spline. If there are less, the point 
    // is the end of the spline. If there are more, there is a fork. 
    // if != 2 we are done with the current spine
    while (ControlPoint1 && ControlPoint1->ConnectedSegments.Num() == 2)
    {
        NextSegment = OtherSegmentAtPoint(ControlPoint1, NextSegment);
        if(outSplineSegments.Contains(NextSegment))
            break;
        outSplineSegments.Add(NextSegment);
        ControlPoint1 = NextSegment->Connections[0].ControlPoint;
    }
    
    // reverse, so that the we can add backward iterated segments
    Algo::Reverse(outSplineSegments);

    // iterate backward from the input segment
    NextSegment = inSegment;
    auto ControlPoint2 = NextSegment->Connections[1].ControlPoint;
    if (!ControlPoint2) return {};

    while (ControlPoint2 && ControlPoint2->ConnectedSegments.Num() == 2)
    {
        NextSegment = OtherSegmentAtPoint(ControlPoint2, NextSegment);
        if(outSplineSegments.Contains(NextSegment))
            break;
        outSplineSegments.Add(NextSegment);
        ControlPoint2 = NextSegment->Connections[1].ControlPoint;
    }
    return outSplineSegments;
}

void ALandscapeSplineToSplineComponent::CopyLandscapeSplineToSplineComponent()
{ 
    auto AddLandscapePointToSplineComponent = [](
        TObjectPtr<ULandscapeSplineSegment> Segment,
        TObjectPtr<ULandscapeSplineControlPoint> Point,
        USplineComponent* MySplineComponent,
        float ArriveTangentLen,
        int PointIdx)
        {
            float LeaveTangentLen =
                Segment->Connections[1].ControlPoint == Point
                ? -Segment->Connections[1].TangentLen
                : Segment->Connections[0].TangentLen;

            auto Location = Point->Location
                + Point->GetOuterULandscapeSplinesComponent()->GetComponentLocation()
                - MySplineComponent->GetComponentLocation();
            auto Rotator = Point->Rotation;
            auto Scale = FVector::One();

            FSplinePoint SplinePoint((float)PointIdx,
                Location,
                Rotator.Vector() * ArriveTangentLen,
                Rotator.Vector() * LeaveTangentLen,
                Rotator,
                Scale,
                ESplinePointType::CurveCustomTangent);

            bool bUpdateSpline = false;
            MySplineComponent->AddPoint(SplinePoint, bUpdateSpline);

            // calculate ArriveTangentLen for the next segment
            ArriveTangentLen =
                Segment->Connections[0].ControlPoint != Point
                ? Segment->Connections[0].TangentLen
                : -Segment->Connections[1].TangentLen;
            return ArriveTangentLen;
        };

    SplineComponent->ClearSplinePoints();

    auto LandscapeSplines = ObtainLandscapeSplinesAsSegments().Array();
    if (LandscapeSplineNumber >= LandscapeSplines.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Incorrect LandscapeSplineNumber"))
        return;
    }

    auto LandscapeSpline = LandscapeSplines[LandscapeSplineNumber];
    if (LandscapeSpline.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No segments in given Spline"))
        return;
    }

    float ArriveTangentLen = 0;
    for (int32 i = 0; i < LandscapeSpline.Num(); ++i)
    {
        auto LandscapeSegment = LandscapeSpline[i];
        // point at the BEGINNING of the segment
        auto LandscapePoint = LandscapeSegment->Connections[0].ControlPoint;
        ArriveTangentLen = AddLandscapePointToSplineComponent(
            LandscapeSegment,
            LandscapePoint,
            SplineComponent,
            ArriveTangentLen,
            i);
    }

    // add the last point
    auto LandscapeSegment = LandscapeSpline[LandscapeSpline.Num() - 1];
    // point at the END of the segment
    auto LandscapePoint = LandscapeSegment->Connections[1].ControlPoint;
    AddLandscapePointToSplineComponent(
        LandscapeSegment,
        LandscapePoint,
        SplineComponent,
        ArriveTangentLen,
        LandscapeSpline.Num());

    SplineComponent->UpdateSpline();

    MoveSplinePoints(SplineComponentSegmentLength, SplineComponentOffset);
}

bool ALandscapeSplineToSplineComponent::AreArraysEqual(const TArray<TObjectPtr<ULandscapeSplineSegment>>& ArrayA, const TArray<TObjectPtr<ULandscapeSplineSegment>>& ArrayB)
{
    if (ArrayA.Num() != ArrayB.Num())
    {
        return false;
    }
    for (int32 Index = 0; Index < ArrayA.Num(); ++Index)
    {
        if (ArrayA[Index] != ArrayB[Index])
        {
            return false; // Arrays are not equal if any elements differ
        }
    }
    return true;
}

TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> ALandscapeSplineToSplineComponent::RemoveReverseDuplicates(
    const TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>>& Splines)
{
    TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> UniqueSplines;

    for (auto& Spline : Splines)
    {
        bool bIsDuplicate = false;

        // Check if the current spline is already in UniqueSplines
        for (auto& UniqueSpline : UniqueSplines)
        {
            TArray<TObjectPtr<ULandscapeSplineSegment>> ReverseSpline = Spline;
            Algo::Reverse(ReverseSpline);
            if (AreArraysEqual(UniqueSpline, ReverseSpline))
            {
                bIsDuplicate = true;
                break;
            }
        }

        // If not a duplicate, add it to UniqueSplines
        if (!bIsDuplicate)
        {
            UniqueSplines.Add(Spline);
        }
    }
    return UniqueSplines;
}

TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> ALandscapeSplineToSplineComponent::ObtainLandscapeSplinesAsSegments()
{
    // at start, all segments are candidates to start a spline
    TSet<TObjectPtr<ULandscapeSplineSegment>> SegmentsToVisit;
    TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> OutSegments;
    if (!IsValid(LandscapeSplineActor))
    {
        UE_LOG(LogSplineConversionActor, Warning, TEXT("No Landscape Actor found in the level!"));
        return OutSegments;
    }
    ULandscapeSplinesComponent* LandscapeSplines = LandscapeSplineActor->GetSplinesComponent();
    for(auto Seg : LandscapeSplines->GetSegments())
    {
        SegmentsToVisit.Add(Seg);
    }

    
    while (!SegmentsToVisit.IsEmpty())
    {
        // get a starting segment of a new spline 
        ULandscapeSplineSegment* Segment = *SegmentsToVisit.CreateConstIterator();

        auto NeighbourSegments = GetNeighbourSegments(Segment);
        for (auto s : NeighbourSegments)
        {
            SegmentsToVisit.Remove(s);
        }
        OutSegments.Add(NeighbourSegments);
        
    }
    OutSegments = RemoveReverseDuplicates(OutSegments);
    return OutSegments;
}

void ALandscapeSplineToSplineComponent::MoveSplinePoints(float SegmentLength, float Offset)
{
    if (!SplineComponent) return;
    if (SegmentLength <= 0) return;
    TArray<FVector> NewPoints;

    // Get the total length of the spline
    float SplineLength = SplineComponent->GetSplineLength();

    // Sample the spline at regular intervals
    for (float Distance = 0.0f; Distance <= SplineLength; Distance += SegmentLength)
    {
        // Get the location and tangent at the current distance
        FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
        FVector Tangent = SplineComponent->GetTangentAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
        Tangent.Normalize();

        // Calculate the perpendicular vector (assuming Z is up)
        FVector Perpendicular = FVector::CrossProduct(Tangent, FVector::UpVector);
        Perpendicular.Normalize();

        // Offset the point by the specified distance
        FVector OffsetLocation = Location + Perpendicular * Offset;

        // Add the new point to the array
        NewPoints.Add(OffsetLocation);
    }

    // Clear the existing spline points
    SplineComponent->ClearSplinePoints();

    // Add the new points to the spline
    for (int32 i = 0; i < NewPoints.Num(); ++i)
    {
        SplineComponent->AddSplinePoint(NewPoints[i], ESplineCoordinateSpace::Local);
    }

    // Update the spline to apply changes
    SplineComponent->UpdateSpline();
}

