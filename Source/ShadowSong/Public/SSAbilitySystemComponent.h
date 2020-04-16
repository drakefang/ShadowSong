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

	int32 GetDefaultAbilityLevel() const;
};
