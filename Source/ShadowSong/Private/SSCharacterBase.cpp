// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "..\Public\SSCharacterBase.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ASSCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

