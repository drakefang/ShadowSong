// Fill out your copyright notice in the Description page of Project Settings.


#include "SSWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASSWeaponBase::ASSWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->RootComponent = Root;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	CapsuleCollision->SetupAttachment(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ASSWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASSWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

