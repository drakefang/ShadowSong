// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSItemBase.h"
#include "SSWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSWeaponItem : public USSItemBase
{
	GENERATED_BODY()
	
public:
	USSWeaponItem()
	{
		ItemType = TEXT("Weapon");
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> WeaponActor;
};
