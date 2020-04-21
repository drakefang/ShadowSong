// Fill out your copyright notice in the Description page of Project Settings.


#include "SSNormalAttackCDCalculation.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SSCharacterBase.h"
#include "SSAttributeSet.h"

float USSNormalAttackCDCalculation::CalculateBaseMagnitude_Implementation(FGameplayEffectSpec const& Spec) const
{
	AActor* Actor = UAbilitySystemBlueprintLibrary::EffectContextGetInstigatorActor(Spec.GetEffectContext());
	ASSCharacterBase* Character = Cast<ASSCharacterBase>(Actor);
	if (Character)
	{
		return -0.5f / 99.f * ((float)Character->GetCharacterLevel()) + 99.5f / 99.f;
	}

	/*float cd = 1.0f;
	FAggregatorEvaluateParameters InEvalParams;
	GetCapturedAttributeMagnitude(
		FGameplayEffectAttributeCaptureDefinition(FGameplayAttribute(FindFieldChecked<UProperty>(USSAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(USSAttributeSet, Level)))
			, EGameplayEffectAttributeCaptureSource::Source, false), Spec, InEvalParams, cd);*/
	return 1.0f;
}
