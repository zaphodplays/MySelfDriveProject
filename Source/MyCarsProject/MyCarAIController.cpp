// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCarAIController.h"

#include "LandscapeSplineToSplineComponent.h"
#include "MyCarsProjectPawn.h"
#include "MySportsCarMovementComponent.h"
#include "TrackSplineDataActor.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRacingAI, Log, All);
// Sets default values
AMyCarAIController::AMyCarAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMyCarAIController::BeginPlay()
{
	Super::BeginPlay();
	LastSplinePointIndex = -1;
	InitializeSplineFromTrackActor();
}

// Called every frame
void AMyCarAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (RacingSpline && IsValid(VehiclePawn))
	{
		DriveCarAlongSpline();
	}
}

void AMyCarAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogRacingAI, Warning, TEXT("Pawn is possessed"));
	if(IsValid(InPawn))
	{
		VehiclePawn = CastChecked<AMyCarsProjectPawn>(InPawn);
	}
}

void AMyCarAIController::InitializeSplineFromTrackActor()
{
	// Find the TrackSplineActor in the level
	ALandscapeSplineToSplineComponent* TrackActor = Cast<ALandscapeSplineToSplineComponent>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ALandscapeSplineToSplineComponent::StaticClass()));

	if (!TrackActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("No TrackSplineActor found in the level!"));
		return;
	}

	// Get the spline component from the track actor
	RacingSpline = TrackActor->SplineComponent;

	if (RacingSpline)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully linked RacingSpline with %d points."),
			   RacingSpline->GetNumberOfSplinePoints());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve SplineComponent from TrackSplineActor!"));
	}
}

void AMyCarAIController::DriveCarAlongSpline(float DeltaSeconds)
{
	if (!RacingSpline || !VehiclePawn) return;
	int32 NextSplinePointIndex = -1;
    // Ensure the car has started moving from a valid index
    if (LastSplinePointIndex == -1)
    {
        NextSplinePointIndex = RacingSpline->FindInputKeyClosestToWorldLocation(VehiclePawn->GetActorLocation());
        NextSplinePointIndex = FMath::Clamp(NextSplinePointIndex, 0, RacingSpline->GetNumberOfSplinePoints() - 1);
    }
	else
	{
		// Get the next spline point index (wrap around if at the end)
		NextSplinePointIndex = (LastSplinePointIndex + 1) % RacingSpline->GetNumberOfSplinePoints();
	}
    

    // Get the next target location
    FVector NextPointLocation = RacingSpline->GetLocationAtSplinePoint(NextSplinePointIndex, ESplineCoordinateSpace::World);

    // Get the car's current location
    FVector CarLocation = VehiclePawn->GetActorLocation();

    // Calculate distance to the next point
    float DistanceToNextPoint = FVector::Dist(CarLocation, NextPointLocation);

    // Check if the car is close enough to the next point
    if (DistanceToNextPoint < 5.0f) // Threshold for "reaching" a point
    {
        LastSplinePointIndex = NextSplinePointIndex; // Update to the next point
        NextSplinePointIndex = (LastSplinePointIndex + 1) % RacingSpline->GetNumberOfSplinePoints(); // Prepare next
    	FSplinePoint NextSplinePoint = RacingSpline->GetSplinePointAt(NextSplinePointIndex, ESplineCoordinateSpace::World);
    	//NextSplinePoint.
        NextPointLocation = RacingSpline->GetLocationAtSplinePoint(NextSplinePointIndex, ESplineCoordinateSpace::World);
    }

    // Calculate the direction vector to the target point
    FVector DirectionToTarget = (NextPointLocation - CarLocation).GetSafeNormal();

    // Get the car's forward vector
    FVector CarForward = VehiclePawn->GetActorForwardVector();

    // Calculate the steering angle by finding the dot product of the car's forward vector and the direction to the target
    float SteeringAngle = FVector::DotProduct(CarForward, DirectionToTarget);

    // Get the ChaosVehicleMovementComponent from the car

	if (UMySportsCarMovementComponent* VehicleMovementComponent = VehiclePawn->GetChaosVehicleMovement(); IsValid(VehicleMovementComponent))
    {
        // Set throttle: If the car is too close to the next point, reduce throttle (braking behavior)
        float ThrottleInput = FMath::Clamp(1.0f - DistanceToNextPoint / 1000.0f, 0.2f, 1.0f); // Throttle decreases as car nears the point

        // Apply throttle input
        VehicleMovementComponent->SetThrottleInput(ThrottleInput);

        // Set steering: Adjust steering angle based on how much the car needs to turn
        float SteeringInput = FMath::Clamp(SteeringAngle, -1.0f, 1.0f); // Steer towards the target direction
        VehicleMovementComponent->SetSteeringInput(SteeringInput);

        // Brake if necessary: Apply brake when the car is very close to the next point (sharp corner)
        if (DistanceToNextPoint < 1.0f)  // Near the point, apply brake to reduce speed
        {
            VehicleMovementComponent->SetBrakeInput(1.0f);  // Full brake
        }
        else
        {
            VehicleMovementComponent->SetBrakeInput(0.0f);  // No brake
        }

        // Debug information
        //UE_LOG(LogTemp, Log, TEXT("Driving: CurrentPoint=%d, NextPoint=%d, TargetLocation=%s, ThrottleInput=%.2f, SteeringInput=%.2f, BrakeInput=%.2f"),
               //LastSplinePointIndex, NextSplinePointIndex, *NextPointLocation.ToString(), ThrottleInput, SteeringInput, VehicleMovementComponent->GetBrakeInput());
    }

    // Optional: Visualize the target point for debugging
    DrawDebugSphere(GetWorld(), NextPointLocation, 50.f, 12, FColor::Yellow, false, 5);
}

void AMyCarAIController::DriveCarAlongSpline()
{
    if (!RacingSpline || !VehiclePawn || !VehiclePawn->GetChaosVehicleMovement())
    {
        UE_LOG(LogTemp, Error, TEXT("Missing required components!"));
        return;
    }

    // Get the car's current position and nearest spline key
    FVector CarLocation = VehiclePawn->GetActorLocation();
    float SplineKey = RacingSpline->FindInputKeyClosestToWorldLocation(CarLocation);

    // Determine the next target location and tangent along the spline
    float SplineLength = RacingSpline->GetSplineLength();
    float CurrentDistance = RacingSpline->GetDistanceAlongSplineAtSplineInputKey(SplineKey);
    float TargetDistance = CurrentDistance + 1500.0f; // Sports car tuned lookahead distance

    if (TargetDistance > SplineLength)
    {
        TargetDistance = FMath::Fmod(TargetDistance, SplineLength); // Loop back for circular tracks
    }

    FVector TargetLocation = RacingSpline->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
    FVector TargetTangent = RacingSpline->GetTangentAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World).GetSafeNormal();

    // Calculate steering angle
    FVector ToTarget = (TargetLocation - CarLocation).GetSafeNormal();
    FVector CarForward = VehiclePawn->GetActorForwardVector();
    float SteeringAngle = FVector::CrossProduct(CarForward, ToTarget).Z;

	UChaosVehicleMovementComponent* VehicleMovement = VehiclePawn->GetChaosVehicleMovement();
		
    VehicleMovement->SetSteeringInput(FMath::Clamp(SteeringAngle, -1.0f, 1.0f));

    // Calculate speed and drift control
    float Speed = VehicleMovement->GetForwardSpeed();
    bool bIsSharpTurn = FMath::Abs(SteeringAngle) > 0.6f; // Tuned sharp turn threshold

    if (bIsSharpTurn) // Drifting logic for tight curves
    {
        VehicleMovement->SetThrottleInput(0.5f); // Moderate throttle during drift
        VehicleMovement->SetBrakeInput(0.0f);
        VehicleMovement->SetHandbrakeInput(true); // Activate handbrake for drift
    }
    else if (Speed < 2000.0f) // Accelerate on straight paths
    {
        VehicleMovement->SetThrottleInput(1.0f); // Full throttle
        VehicleMovement->SetBrakeInput(0.0f);
        VehicleMovement->SetHandbrakeInput(false);
    }
    else // Maintain speed on gentle turns
    {
        VehicleMovement->SetThrottleInput(0.8f); // Slight throttle
        VehicleMovement->SetBrakeInput(0.1f); // Gentle braking to stabilize
        VehicleMovement->SetHandbrakeInput(false);
    }

	// Debug visualization
	// Optional: Visualize the target point for debugging
	DrawDebugSphere(GetWorld(), TargetLocation, 50.f, 12, FColor::Yellow, false, 5);
	DrawDebugLine(GetWorld(), CarLocation, TargetLocation, FColor::Green, false, 0.1f, 0, 2.0f);
}



