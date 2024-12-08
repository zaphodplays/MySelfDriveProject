// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackSplineDataActor.h"

#include "EngineUtils.h"
#include "Landscape.h"
#include "LandscapeSplineActor.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplinesComponent.h"
#include "LandscapeSplineSegment.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSplineActor, Log, All);
// Sets default values
ATrackSplineDataActor::ATrackSplineDataActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Initialize the SplineComponent
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("TrackSpline"));
	SplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATrackSplineDataActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogSplineActor, Log, TEXT("TrackSplineActor: Initializing spline points from landscape."));

	// Initialize the spline from the landscape
	//InitializeSplineFromLandscape();

	if (SplineComponent->GetNumberOfSplinePoints() > 0)
	{
		UE_LOG(LogSplineActor, Log, TEXT("TrackSpline successfully initialized with %d points."),
			   SplineComponent->GetNumberOfSplinePoints());
	}
	else
	{
		UE_LOG(LogSplineActor, Warning, TEXT("Failed to initialize TrackSpline!"));
	}
	
}

// Called every frame
void ATrackSplineDataActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATrackSplineDataActor::InitializeSplineFromLandscape()
{
	// Find the Landscape Actor in the level
	ALandscapeSplineActor* LandscapeSplineActor = nullptr;
	for (TActorIterator<ALandscapeSplineActor> It(GetWorld()); It; ++It)
	{
		LandscapeSplineActor = *It;
		break; // Assuming only one landscape exists
	}

	if (!IsValid(LandscapeSplineActor))
	{
		UE_LOG(LogSplineActor, Warning, TEXT("No Landscape Actor found in the level!"));
		return;
	}

	ULandscapeSplinesComponent* LandscapeSplines = LandscapeSplineActor->GetSplinesComponent();

	// Iterate over all segments in the LandscapeSplinesComponent
    const TArray<ULandscapeSplineSegment*>& Segments = LandscapeSplines->GetSegments();

	int i = 0;
	for (ULandscapeSplineSegment* Segment : Segments)
	{
		if (!Segment)
		{
			continue;
		}

		

		// Get control points from each connection (start and end)
		ULandscapeSplineControlPoint* StartControlPoint = Segment->Connections[0].ControlPoint;
		ULandscapeSplineControlPoint* EndControlPoint = Segment->Connections[1].ControlPoint;
        
		if (!StartControlPoint || !EndControlPoint)
		{
			continue;
		}

		// Define the socket names for the start and end connections
		
		FName StartSocketName = Segment->Connections[0].SocketName; // Assumed socket name for start
		FName EndSocketName = Segment->Connections[1].SocketName;     // Assumed socket name for end

		// Initialize variables for location and rotation
		FVector StartLocation, EndLocation;
		FRotator StartRotation, EndRotation;

		// Get the location and rotation for the start connection
		
		StartControlPoint->GetConnectionLocationAndRotation(StartSocketName, StartLocation, StartRotation);

		// Get the location and rotation for the end connection
		EndControlPoint->GetConnectionLocationAndRotation(EndSocketName, EndLocation, EndRotation);

		// Calculate tangents based on the difference in locations (direction from one point to the other)
		FVector StartTangent = (EndLocation - StartLocation).GetSafeNormal();
		FVector EndTangent = -StartTangent;  // The tangent direction for the next point is the reverse

		// Add the start point to the spline
		SplineComponent->AddSplinePoint(StartLocation, ESplineCoordinateSpace::World, false);
		SplineComponent->SetTangentAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, StartTangent, ESplineCoordinateSpace::World);

		// Add the end point to the spline
		SplineComponent->AddSplinePoint(EndLocation, ESplineCoordinateSpace::World, false);
		SplineComponent->SetTangentAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, EndTangent, ESplineCoordinateSpace::World);

		// Optional Debug: visualize points
		DrawDebugSphere(GetWorld(), (StartLocation+EndLocation)/2.f, 50.0f, 12, FColor::Green, true, -1.0f);
		//DrawDebugSphere(GetWorld(), EndLocation, 50.0f, 12, FColor::Red, true, -1.0f);
	}
    
    

	
	
	// Finalize the spline setup
	SplineComponent->UpdateSpline();
	OnSplineDataConstructed.Broadcast(SplineComponent);
}

