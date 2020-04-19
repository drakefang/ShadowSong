// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

#include "SSTypes.h"


SHADOWSONG_API DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);



UENUM(BlueprintType)
enum class ESSAbilityBindActionID : uint8
{
	None		UMETA(DisplayName = "None"),
	Sprint		UMETA(DisplayName = "Sprint"),
	Jump		UMETA(DisplayName = "Jump"),
	Dash		UMETA(DisplayName = "Dash"),
	Action1		UMETA(DisplayName = "Action1"),
	Action2		UMETA(DisplayName = "Action2"),
	Action3		UMETA(DisplayName = "Action3"),
	Action4		UMETA(DisplayName = "Action4"),
	Action5		UMETA(DisplayName = "Action5"),
	Action6		UMETA(DisplayName = "Action6"),
	Action7		UMETA(DisplayName = "Action7"),
	Action8		UMETA(DisplayName = "Action8"),
	Action9		UMETA(DisplayName = "Action9"),
	Action0		UMETA(DisplayName = "Action0"),
};

UENUM(BlueprintType)
enum class ESSAbilityID : uint8
{
	None				UMETA(DisplayName = "None"),
	Sprint				UMETA(DisplayName = "Sprint"),
	Jump				UMETA(DisplayName = "Jump"),
	Dash				UMETA(DisplayName = "Dash"),
	SwordShieldAttack	UMETA(DisplayName = "SwordShieldAttack"),
};