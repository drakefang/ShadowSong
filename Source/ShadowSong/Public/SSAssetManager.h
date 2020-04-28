// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SSAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	USSAssetManager() {}

	virtual void StartInitialLoading() override;

	static const FPrimaryAssetType EquipmentItemType;
	static const FPrimaryAssetType SkillItemType;
	static const FPrimaryAssetType PotionItemType;
	static const FPrimaryAssetType TokenItemType;

	static USSAssetManager& Get();

	class USSItemBase* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId);
};
