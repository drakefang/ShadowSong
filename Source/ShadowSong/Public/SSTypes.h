// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableRenderAsset.h"
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

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	VelocityDirection,
	LookingDirection
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

USTRUCT(BlueprintType)
struct FMeshPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName Socket = "None";
};

USTRUCT(BlueprintType)
struct FMeshPartRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Socket = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<UStreamableRenderAsset> DefaultMesh;
};