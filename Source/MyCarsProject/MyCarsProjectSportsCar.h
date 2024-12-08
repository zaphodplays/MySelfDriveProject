// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyCarsProjectPawn.h"
#include "MyCarsProjectSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class MYCARSPROJECT_API AMyCarsProjectSportsCar : public AMyCarsProjectPawn
{
	GENERATED_BODY()
	
public:

	AMyCarsProjectSportsCar(const FObjectInitializer& ObjectInitializer);
};
