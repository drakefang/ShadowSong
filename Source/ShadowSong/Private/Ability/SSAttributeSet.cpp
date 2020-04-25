// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

USSAttributeSet::USSAttributeSet() 
	: Health(1.f)
	, MaxHealth(1.f)
	, Mana(0)
	, MaxMana(0)
	, Damage(0)
	, AttackPower(1.f)
	, DefensePower(1.f)
	, Level(1)
{

}

void USSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		//AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		//AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
}

void USSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FString cs = Data.EffectSpec.GetContext().ToString();
	UE_LOG(LogGame, Display, TEXT("%s:%s"), __FUNCTION__, *cs);
}

void USSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USSAttributeSet, Health);
	DOREPLIFETIME(USSAttributeSet, MaxHealth);
	DOREPLIFETIME(USSAttributeSet, Mana);
	DOREPLIFETIME(USSAttributeSet, MaxMana);
	DOREPLIFETIME(USSAttributeSet, AttackPower);
	DOREPLIFETIME(USSAttributeSet, DefensePower);
	DOREPLIFETIME(USSAttributeSet, MoveSpeed);
	DOREPLIFETIME(USSAttributeSet, Stamina);
	DOREPLIFETIME(USSAttributeSet, MaxStamina);
}

void USSAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Health);
}

void USSAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxHealth);
}

void USSAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Mana);
}

void USSAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxMana);
}

void USSAttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MoveSpeed);
}

void USSAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, AttackPower);
}

void USSAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, DefensePower);
}

void USSAttributeSet::OnRep_Stamina()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Stamina);
}

void USSAttributeSet::OnRep_MaxStamina()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxStamina);
}

void USSAttributeSet::OnRep_Level()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Level);
}
