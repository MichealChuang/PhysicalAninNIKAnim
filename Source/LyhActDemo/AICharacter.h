// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HumanLike.h"
#include "AICharacter.generated.h"

UCLASS(config = Game)
class AAICharacter : public AHumanLike
{
	GENERATED_BODY()
public:
	AAICharacter();
	
};

