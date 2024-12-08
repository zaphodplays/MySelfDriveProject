// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "MyCarAIController.generated.h"

class USplineComponent;
class AMyCarsProjectPawn;

UCLASS()
class MYCARSPROJECT_API AMyCarAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyCarAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Index of the last covered spline point
	int32 LastSplinePointIndex;

	UPROPERTY()
	TObjectPtr<USplineComponent> RacingSpline;

	/** Pointer to the controlled vehicle pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AMyCarsProjectPawn> VehiclePawn;
	
	virtual void OnPossess(APawn* InPawn) override;
	

	// Function to find the TrackSplineActor and set the RacingSpline
	void InitializeSplineFromTrackActor();

	// Function to drive the car along the spline
	void DriveCarAlongSpline(float DeltaSeconds);

	void DriveCarAlongSpline();
};
