// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AICharacter.h"
#include "ConstructorHelpers.h"
#include "PlayerStats.h"


AAICharacter::AAICharacter()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AIDataObject(TEXT("/Game/Common/stats"));
	if (AIDataObject.Object)
	{
		Stats = *AIDataObject.Object->FindRow<FPlayerStats>(TEXT("AI"), TEXT(""));
	}
}

