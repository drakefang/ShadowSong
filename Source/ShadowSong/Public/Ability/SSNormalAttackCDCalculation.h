// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "SSNormalAttackCDCalculation.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSNormalAttackCDCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

	virtual float CalculateBaseMagnitude_Implementation(FGameplayEffectSpec const& Spec) const override;
};
