// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SSCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USSCharacterMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimSpeedMultiplier;

	uint8 bRequestToStartSprinting : 1;
	uint8 bRequestToStartAim : 1;

	float GetMaxSpeed() const override;

	void UpdateFromCompressedFlags(uint8 Flags) override;

	class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	UFUNCTION(BlueprintCallable, Category = "Aim")
	void StartAim();

	UFUNCTION(BlueprintCallable, Category = "Aim")
	void StopAim();
};
