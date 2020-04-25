// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SSAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	USSAbilitySystemComponent();
	~USSAbilitySystemComponent();

	bool StartupEffectApplied = false;
	bool StartupAbilitiesGiven = false;

	int32 GetDefaultAbilityLevel() const;

	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, 
		TArray<class USSGameplayAbility*>& ActiveAbilities) const;

	static USSAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
};
