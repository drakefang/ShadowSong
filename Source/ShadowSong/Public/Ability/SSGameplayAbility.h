// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowSong/ShadowSong.h"
#include "Abilities/GameplayAbility.h"
#include "SSGameplayAbility.generated.h"


USTRUCT(BlueprintType)
struct FSSGamePlayEffectContainer
{
	GENERATED_BODY()

public:
	FSSGamePlayEffectContainer() {}

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect Container")
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
};

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USSGameplayAbility();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	ESSAbilityBindActionID InputID = ESSAbilityBindActionID::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	ESSAbilityID AbilityID = ESSAbilityID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	TMap<FGameplayTag, FSSGamePlayEffectContainer> EffectMap;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
