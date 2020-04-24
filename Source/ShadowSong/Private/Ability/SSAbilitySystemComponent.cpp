// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAbilitySystemComponent.h"
#include "SSCharacterBase.h"
#include "SSGameplayAbility.h"


USSAbilitySystemComponent::USSAbilitySystemComponent()
{
}

USSAbilitySystemComponent::~USSAbilitySystemComponent()
{
}

int32 USSAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ASSCharacterBase* OwningCharacter = Cast<ASSCharacterBase>(OwnerActor);
	if (OwningCharacter)
	{
		return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

void USSAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, 
	TArray<class USSGameplayAbility*>& ActiveAbilities) const
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<USSGameplayAbility>(ActiveAbility));
		}
	}
}
