// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyCarsProjectPlayerController.h"
#include "MyCarsProjectPawn.h"
#include "MyCarsProjectUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "MySportsCarMovementComponent.h"
#include "Net/UnrealNetwork.h"

void AMyCarsProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, GetNetMode() == NM_Client ? FColor::Yellow : FColor::Red, TEXT("BeginPlay called for role = %s"), *(GetNetMode() == NM_Client ? "Client" : "Server"));
	// spawn the UI widget and add it to the viewport
	if(this->IsLocalController())
	{
		VehicleUI = CreateWidget<UMyCarsProjectUI>(this, VehicleUIClass);

		check(VehicleUI);
		
		VehicleUI->AddToViewport();
	}
	
	
}

void AMyCarsProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void AMyCarsProjectPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);
	
	//if(this->HasAuthority() && IsValid(VehiclePawn))
	///{
		//const float CurrSpeed = VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed();
		//const int32 CurrGear = VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear();
		//ClientUpdateUI(CurrSpeed, CurrGear);
	//}
	/*
	else if(IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		const float CurrSpeed = VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeedMPH();
		const int32 CurrGear = VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear();
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Speed = %f"), CurrSpeed);
		VehicleUI->UpdateSpeed(10);
		VehicleUI->UpdateGear(2);
	}
	else if(!this->HasAuthority() && IsValid(VehiclePawn))
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Non authority print"));
	}*/
	
}

void AMyCarsProjectPlayerController::ClientUpdateUI_Implementation(const float Speed, const int32 Gear)
{
	
	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(Speed);
		VehicleUI->UpdateGear(Gear);
	}
	
}

void AMyCarsProjectPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, GetNetMode() == NM_Client ? FColor::Yellow : FColor::Red, TEXT("On Possess called for role = %s"), *( FString(GetNetMode() == NM_Client ? "Client" : "Server")));
	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<AMyCarsProjectPawn>(InPawn);
}


void AMyCarsProjectPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = false;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(AMyCarsProjectPlayerController, VehiclePawn, Params);
}

void AMyCarsProjectPlayerController::OnRep_VehiclePawn()
{
	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, GetNetMode() == NM_Client ? FColor::Yellow : FColor::Red, TEXT("On Rep Vehicle Pawn called for role = %s"), *FString((GetNetMode() == NM_Client ? "Client" : "Server")));
}
