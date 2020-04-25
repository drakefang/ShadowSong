// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterMovementComponent.h"
#include "SSCharacterBase.h"

void USSCharacterMovementComponent::FSSSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartADS = false;
	SavedRequestToStartSprinting = false;
}

uint8 USSCharacterMovementComponent::FSSSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool USSCharacterMovementComponent::FSSSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character,
	float MaxDelta) const
{
	if (SavedRequestToStartSprinting != ((FSSSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartADS != ((FSSSavedMove*)&NewMove)->SavedRequestToStartADS)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void USSCharacterMovementComponent::FSSSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->bRequestToStartSprinting;
		SavedRequestToStartADS = CharacterMovement->bRequestToStartAim;
	}
}

void USSCharacterMovementComponent::FSSSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	USSCharacterMovementComponent* CharacterMovement = Cast<USSCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

USSCharacterMovementComponent::FSSNetworkPredictionData_Client::FSSNetworkPredictionData_Client(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr USSCharacterMovementComponent::FSSNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FSSSavedMove);
}

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
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		USSCharacterMovementComponent* MutableThis = const_cast<USSCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FSSNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
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
