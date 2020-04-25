// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
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

void USSAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	ClientSaveActivateAbilityHandles();
}

void USSAbilitySystemComponent::ClientSaveActivateAbilityHandles_Implementation()
{
	ASSCharacterBase* Owner = Cast<ASSCharacterBase>(GetOwner());
	
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		Owner->SaveActivateAbilityHandles(Spec);
	}
}

USSAbilitySystemComponent* USSAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
                                                                                         bool LookForComponent)
{
	return Cast<USSAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
