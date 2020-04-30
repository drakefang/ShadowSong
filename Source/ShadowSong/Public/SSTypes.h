// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable All
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
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
enum class EWeaponAnimType : uint8
{
	WAT_NoWeapon			UMETA(DisplayName = "NoWeapon"),
	WAT_Bow					UMETA(DisplayName = "Bow"),
	WAT_DoubleSword			UMETA(DisplayName = "DoubleSword"),
	WAT_MagicWand			UMETA(DisplayName = "MagicWand"),
	WAT_SwordShield			UMETA(DisplayName = "SwordShield"),
	WAT_TwoHandSword		UMETA(DisplayName = "TwoHandSword"),
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
struct FSkeletalPartRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPartType Part;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TAssetPtr<USkeletalMesh> DefaultMesh;
};

USTRUCT(BlueprintType)
struct FWeaponSocketRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPartType Part;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Socket;
};

USTRUCT(BlueprintType)
struct SHADOWSONG_API FItemData
{
	GENERATED_BODY()

	FItemData()
		: ItemCount(1)
		, ItemLevel(1)
	{

	}

	FItemData(int32 InItemCount, int32 InItemLevel)
		: ItemCount(InItemCount)
		, ItemLevel(InItemLevel)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemLevel;

	bool operator==(const FItemData& Other) const
	{
		return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel;
	}
	bool operator!=(const FItemData& Other) const
	{
		return !(*this == Other);
	}

	bool IsValid() const
	{
		return ItemCount > 0;
	}

	void UpdateItemData(const FItemData& Other, int32 MaxCount, int32 MaxLevel)
	{
		if (MaxCount <= 0)
		{
			MaxCount = MAX_int32;
		}

		if (MaxLevel <= 0)
		{
			MaxLevel = MAX_int32;
		}

		ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
		ItemLevel = FMath::Clamp(Other.ItemLevel, 1, MaxLevel);
	}
};