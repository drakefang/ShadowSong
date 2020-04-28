// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSAssetManager.h"
#include "SSEquipmentItem.h"
#include "SSItemBase.h"
#include "SSWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSWeaponItem : public USSEquipmentItem
{
	GENERATED_BODY()
	
public:
	USSWeaponItem()
	{
		
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> WeaponActor;
};
