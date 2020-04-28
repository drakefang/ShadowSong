// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAssetManager.h"
#include "SSItemBase.h"

#include "AbilitySystemGlobals.h"
#include "ShadowSong/ShadowSong.h"
#include "Engine/Engine.h"

const FPrimaryAssetType USSAssetManager::EquipmentItemType = TEXT("Equipment");
const FPrimaryAssetType USSAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType USSAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType USSAssetManager::TokenItemType = TEXT("Token");

void USSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

USSAssetManager& USSAssetManager::Get()
{
	USSAssetManager* This = Cast<USSAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogGame, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be USSAssetManager!"));
		return *NewObject<USSAssetManager>(); // never calls this
	}
}

USSItemBase* USSAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	USSItemBase* LoadedItem = Cast<USSItemBase>(ItemPath.TryLoad());

	if (LoadedItem == nullptr)
	{
		UE_LOG(LogGame, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
