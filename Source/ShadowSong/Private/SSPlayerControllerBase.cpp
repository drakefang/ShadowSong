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
	LocationInterpSpeed = 10.0f;
	RotationInterpSpeed = 5.0f;
	EnableRotationInterp = true;
	RealTimeSpeed = 100.0f;
}

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

void ASSPlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayer(DeltaTime);
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

FRotator ASSPlayerControllerBase::GetMoveRotator() const
{
	if (UKismetSystemLibrary::IsValid(HeroRef))
	{
		FRotator rot = HeroRef->GetActorRotation();
		return UKismetMathLibrary::MakeRotator(0, 0, rot.Yaw);
	}
	return FRotator();
}

void ASSPlayerControllerBase::SetDestination(bool Pressed)
{
	if (Pressed)
	{
		if (OnMouseClick())
		{
			AActor* Click = GetWorld()->SpawnActor(ClickClass, &ClickLocation);
			Click->SetLifeSpan(ClickLife);
			UpdatePlayerOnServer(ClickLocation);
		}
	}
}

bool ASSPlayerControllerBase::OnMouseClick()
{
	FHitResult Result;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, Result);
	if (!Result.bBlockingHit)
	{
		return false;
	}
	ClickLocation = Result.Location;
	return true;
}

void ASSPlayerControllerBase::UpdatePlayer(float DeltaTime)
{
	if (!UKismetSystemLibrary::IsValid(HeroRef))
	{
		return;
	}

	FVector SvrLocation = HeroRef->GetActorLocation();
	FVector ClientLocation = ClientPawnRef->GetActorLocation();
	FVector NewLocation = UKismetMathLibrary::VInterpTo(ClientLocation, SvrLocation, DeltaTime, LocationInterpSpeed);
	ClientPawnRef->SetActorLocation(NewLocation);

	if (EnableRotationInterp)
	{
		FRotator SvrRot = HeroRef->GetActorRotation();
		FRotator ClientRot = ClientPawnRef->GetActorRotation();
		FRotator NewRot = UKismetMathLibrary::RInterpTo_Constant(ClientRot, SvrRot, DeltaTime, RotationInterpSpeed);
		ClientPawnRef->SetActorRotation(NewRot);
	}
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
	ServerControllerRef->PlayerControllerRef = this;
	ServerControllerRef->Possess(HeroRef);
}

bool ASSPlayerControllerBase::SpawnPlayerOnServer_Validate()
{
	return true;
}

void ASSPlayerControllerBase::UpdatePlayerOnServer_Implementation(FVector Click)
{
	if (!HasAuthority())
	{
		return;
	}
	FVector location = HeroRef->GetActorLocation();
	FVector offset = Click - location;
	if (UKismetMathLibrary::VSize(offset) >= MinClickDistance && UKismetSystemLibrary::IsValid(ServerControllerRef))
	{
		ServerControllerRef->MoveToLocation(Click);
	}
}

bool ASSPlayerControllerBase::UpdatePlayerOnServer_Validate(FVector Click)
{
	return true;
}

void ASSPlayerControllerBase::MoveForwardOnServer_Implementation(float ScaleValue, float DeltaTime, float Speed)
{
	FRotator rot = GetMoveRotator();
	FVector delta = UKismetMathLibrary::GetForwardVector(rot);
	delta = delta * Speed * DeltaTime;
	if (ScaleValue < 0)
	{
		delta = delta * 0.5f;
	}
	HeroRef->AddMovementInput(delta, ScaleValue);
}

bool ASSPlayerControllerBase::MoveForwardOnServer_Validate(float ScaleValue, float DeltaTime, float Speed)
{
	return true;
}

void ASSPlayerControllerBase::MoveRightOnServer_Implementation(float ScaleValue, float DeltaTime, float Speed)
{
	FRotator rot = GetMoveRotator();
	FVector delta = UKismetMathLibrary::GetRightVector(rot);
	delta = delta * Speed * DeltaTime;
	HeroRef->AddMovementInput(delta, ScaleValue);
}

bool ASSPlayerControllerBase::MoveRightOnServer_Validate(float ScaleValue, float DeltaTime, float Speed)
{
	return true;
}