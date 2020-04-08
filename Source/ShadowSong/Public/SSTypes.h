// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SSTypes.generated.h"


UENUM(BlueprintType)
enum class EPartType : uint8
{
	PT_Cloth		UMETA(DisplayName = "Cloth"),
	PT_Face			UMETA(DisplayName = "Face"),
	PT_Hair			UMETA(DisplayName = "Hair"),
	PT_Glove		UMETA(DisplayName = "Glove"),
	PT_Shoe			UMETA(DisplayName = "Shoe"),
	PT_Shoulder		UMETA(DisplayName = "Shoulder"),
	PT_Helm			UMETA(DisplayName = "Helm"),
	PT_Belt			UMETA(DisplayName = "Belt"),
	PT_Shield		UMETA(DisplayName = "Shield"),
	PT_LeftWeapon	UMETA(DisplayName = "LeftWeapon"),
	PT_RightWeapon	UMETA(DisplayName = "RightWeapon"),
	PT_Backpack		UMETA(DisplayName = "Backpack"),
};

USTRUCT(BlueprintType)
struct FCurrentSelectPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPartType PartType	UMETA(DisplayName = "PartType");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MeshStr		UMETA(DisplayName = "MeshName");
};