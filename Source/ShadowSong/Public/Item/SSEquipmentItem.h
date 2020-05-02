// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/SSItemBase.h"
#include "SSAssetManager.h"
#include "SSTypes.h"
#include "SSEquipmentItem.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSEquipmentItem : public USSItemBase
{
	GENERATED_BODY()

public:
	USSEquipmentItem()
	{
		ItemType = USSAssetManager::EquipmentItemType;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Part")
	EPartType EquipPartType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Part")
	EWeaponAnimType WeaponAnimType;
};
