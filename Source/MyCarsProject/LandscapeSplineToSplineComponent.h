#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "LandscapeSplineSegment.h"
#include "LandscapeSplineToSplineComponent.generated.h"

UCLASS()
class MYCARSPROJECT_API ALandscapeSplineToSplineComponent : public AActor
{
	GENERATED_BODY()
 
public: 
	ALandscapeSplineToSplineComponent();

public:  
	UFUNCTION(CallInEditor, Category = "Landscape Spline To Spline Component")
	void CopyLandscapeSplineToSplineComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landscape Spline To Spline Component")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Spline To Spline Component ")
	int32 LandscapeSplineNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Spline To Spline Component ")
	float SplineComponentOffset = 0; 
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Spline To Spline Component ")
	float SplineComponentSegmentLength = 1000;

	UPROPERTY(EditInstanceOnly, Category = "Landscape Spline to Spline Component")
	ALandscapeSplineActor* LandscapeSplineActor = nullptr;

private:
	TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> ObtainLandscapeSplinesAsSegments();
 
	TArray<TObjectPtr<ULandscapeSplineSegment>> GetNeighbourSegments(
	 const TObjectPtr <ULandscapeSplineSegment> inSegment);

	bool AreArraysEqual(
	 const TArray<TObjectPtr<ULandscapeSplineSegment>>& ArrayA,
	 const TArray<TObjectPtr<ULandscapeSplineSegment>>& ArrayB);

	TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>> RemoveReverseDuplicates(
	 const TSet<TArray<TObjectPtr<ULandscapeSplineSegment>>>& Splines);

	void MoveSplinePoints(float SegmentLength, float Offset);
};
