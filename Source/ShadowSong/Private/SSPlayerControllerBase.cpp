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


ASSPlayerControllerBase::ASSPlayerControllerBase()
{
	
}

void ASSPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	DisableInput(this);

	FLatentActionInfo action;
	action.CallbackTarget = this;
	action.ExecutionFunction = "InitPlayer";
	action.Linkage = 0;
	action.UUID = USSHelper::GenerateUUID(); //TODO
	UKismetSystemLibrary::Delay(GetWorld(), 0.5f, action);
}

void ASSPlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void ASSPlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}

void ASSPlayerControllerBase::InitPlayer()
{
	APawn* P = this->GetPawn();
	HeroRef = Cast<ASSCharacterBase>(P);
	EnableInput(this);
}

FTransform ASSPlayerControllerBase::GetSpawnRandomTransform() const
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