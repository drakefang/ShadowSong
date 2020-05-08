// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowSong/ShadowSong.h"
#include "Abilities/GameplayAbility.h"
#include "SSGameplayAbility.generated.h"


USTRUCT(BlueprintType)
struct FSSGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FSSGameplayEffectContainer() {}

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect Container")
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
};

USTRUCT(BlueprintType)
struct FSSGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FSSGameplayEffectContainerSpec() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetEffectSpecs;

	bool HasValidEffects() const;

	bool HasValidTargets() const;

	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
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
	TMap<FGameplayTag, FSSGameplayEffectContainer> EffectMap;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FSSGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FSSGameplayEffectContainer& ContainerSpec, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FSSGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FSSGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
};
