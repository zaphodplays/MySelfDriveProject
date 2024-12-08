// Fill out your copyright notice in the Description page of Project Settings.


#include "MySportsCarMovementComponent.h"


// Sets default values for this component's properties
UMySportsCarMovementComponent::UMySportsCarMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;
	//ChaosWheeledVehicle is the base wheeled vehicle pawn actor. By default it uses UChaosWheeledVehicleMovementComponent for its simulation, but this can be overridden by inheriting from the class and modifying its constructor like so:
	//Super(ObjectInitializer.SetDefaultSubobjectClass<UMySportsCarMovementComponent>(ChaosVehicleMovement));
	// ...
}


// Called when the game starts
void UMySportsCarMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMySportsCarMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

