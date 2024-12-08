// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackSplineDataActor.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplineDataConstructed, USplineComponent*, Splines);

UCLASS(Blueprintable)
class MYCARSPROJECT_API ATrackSplineDataActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrackSplineDataActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FOnSplineDataConstructed OnSplineDataConstructed;

	// Spline component representing the track
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track", meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// Initialize spline points from landscape splines
	void InitializeSplineFromLandscape();
};
