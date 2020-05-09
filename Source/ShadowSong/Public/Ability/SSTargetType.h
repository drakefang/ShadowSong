// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "SSTargetType.generated.h"


class ASSCharacterBase;

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSONG_API USSTargetType : public UObject
{
	GENERATED_BODY()

public:
	USSTargetType() {}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ASSCharacterBase* TargetingCharacter, AActor* TargetingActor, 
		FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintable)
class SHADOWSONG_API USSTargetType_UseOwner : public USSTargetType
{
	GENERATED_BODY()
public:
	USSTargetType_UseOwner() {}

	void GetTargets_Implementation(ASSCharacterBase* TargetingCharacter, AActor* TargetingActor,
	                               FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS()
class USSTargetType_UseEventData : public USSTargetType
{
	GENERATED_BODY()
public:
	USSTargetType_UseEventData() {}

	void GetTargets_Implementation(ASSCharacterBase* TargetingCharacter, AActor* TargetingActor,
	                               FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
