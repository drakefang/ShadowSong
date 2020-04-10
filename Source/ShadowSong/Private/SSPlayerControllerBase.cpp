// Fill out your copyright notice in the Description page of Project Settings.


#include "SSPlayerControllerBase.h"
#include "SSHelper.h"
#include "SSPawnBase.h"
#include "SSCharacterBase.h"
#include "SSPlayerStateBase.h"
#include "SSAIControllerBase.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/KismetMathLibrary.h"

void ASSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	DisableInput(this);

	FLatentActionInfo action;
	action.CallbackTarget = this;
	action.ExecutionFunction = "InitPlayer";
	action.Linkage = 0;
	action.UUID = USSHelper::GenerateUUID(); //TODO
	UKismetSystemLibrary::Delay(GetWorld(), 0.5f, action);
}

void ASSPlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSPlayerControllerBase, ClientPawnRef);
	DOREPLIFETIME(ASSPlayerControllerBase, HeroRef);
}

void ASSPlayerControllerBase::InitPlayer()
{
	if (!IsLocalPlayerController())
	{
		return;
	}
	bShowMouseCursor = true;

	InitPlayerOnServer();

	EnableInput(this);
}

FTransform ASSPlayerControllerBase::GetSpawnTransform() const
{
	TSubclassOf<APlayerStart> ClassOfPlayerStart = APlayerStart::StaticClass();
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassOfPlayerStart, Starts);
	int LastIndex = Starts.Num() - 1;
	if (LastIndex >= 0)
	{
		int index = UKismetMathLibrary::RandomIntegerInRange(0, LastIndex);
		AActor* Start = Starts[index];
		return Start->GetActorTransform();
	}
	return FTransform();
}

void ASSPlayerControllerBase::InitPlayerOnServer_Implementation()
{
	APawn* pawn = this->GetPawn();
	ClientPawnRef = Cast<ASSPawnBase>(pawn);
	if (ClientPawnRef)
	{
		ClientPawnRef->PlayerControllerRef = this;
		SpawnPlayerOnServer();
	}
}

bool ASSPlayerControllerBase::InitPlayerOnServer_Validate()
{
	return true;
}

void ASSPlayerControllerBase::SpawnPlayerOnServer_Implementation()
{
	if (!this->HasAuthority())
	{
		return;
	}
	FTransform SpawnTransform = GetSpawnTransform();
	ClientPawnRef->SetActorTransform(SpawnTransform);
	FActorSpawnParameters SpwanParams;
	SpwanParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	HeroRef = GetWorld()->SpawnActor<ASSCharacterBase>(HeroClass, SpawnTransform, SpwanParams);

	ASSPlayerStateBase* ps = Cast<ASSPlayerStateBase>(this->PlayerState);
	ps->HeroRef = HeroRef;

	if (!UKismetSystemLibrary::IsValid(ServerControllerRef))
	{
		ServerControllerRef = GetWorld()->SpawnActor<ASSAIControllerBase>(ServerControllerClass, SpawnTransform, SpwanParams);
	}
	ServerControllerRef->HeroRef = HeroRef;
	ServerControllerRef->Possess(HeroRef);
}

bool ASSPlayerControllerBase::SpawnPlayerOnServer_Validate()
{
	return true;
}