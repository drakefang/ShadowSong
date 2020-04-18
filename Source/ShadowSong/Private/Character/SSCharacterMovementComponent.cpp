// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterMovementComponent.h"
#include "SSCharacterBase.h"

USSCharacterMovementComponent::USSCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	AimSpeedMultiplier = 0.5f;
}

float USSCharacterMovementComponent::GetMaxSpeed() const
{
	ASSCharacterBase* Owner = Cast<ASSCharacterBase>(GetOwner());
	if (!Owner)
	{
		return Super::GetMaxSpeed();
	}

	if (bRequestToStartSprinting)
	{
		//UE_LOG(LogTemp, Display, TEXT("===>%s(), %f<==="), *FString(__FUNCTION__), SprintSpeedMultiplier);
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (bRequestToStartAim)
	{
		return Owner->GetMoveSpeed() * AimSpeedMultiplier;
	}
	return Owner->GetMoveSpeed();
}

void USSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

}

FNetworkPredictionData_Client* USSCharacterMovementComponent::GetPredictionData_Client() const
{
	return nullptr;
}

void USSCharacterMovementComponent::StartSprinting()
{
	bRequestToStartSprinting = true;
	//UE_LOG(LogTemp, Display, TEXT("===>%s()<==="), *FString(__FUNCTION__));
}

void USSCharacterMovementComponent::StopSprinting()
{
	bRequestToStartSprinting = false;
}

void USSCharacterMovementComponent::StartAim()
{
	bRequestToStartAim = true;
}

void USSCharacterMovementComponent::StopAim()
{
	bRequestToStartAim = false;
}
