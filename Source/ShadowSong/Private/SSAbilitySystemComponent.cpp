// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAbilitySystemComponent.h"
#include "SSCharacterBase.h"


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
