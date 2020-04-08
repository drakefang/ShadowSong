// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Face = CreateMeshPart<USkeletalMeshComponent>(TEXT("Face"));
	Hair = CreateMeshPart<USkeletalMeshComponent>(TEXT("Hair"));
	Glove = CreateMeshPart<USkeletalMeshComponent>(TEXT("Glove"));
	Shoe = CreateMeshPart<USkeletalMeshComponent>(TEXT("Shoe"));
	Shoulder = CreateMeshPart<USkeletalMeshComponent>(TEXT("Shoulder"));
	Helm = CreateMeshPart<USkeletalMeshComponent>(TEXT("Helm"));
	Belt = CreateMeshPart<USkeletalMeshComponent>(TEXT("Belt"));

	Shield = CreateMeshPart<UStaticMeshComponent>(TEXT("Shield"), "hand_l");
	LeftWeapon = CreateMeshPart<UStaticMeshComponent>(TEXT("LeftWeapon"), "hand_lSocket");
	RightWeapon = CreateMeshPart<UStaticMeshComponent>(TEXT("RightWeapon"), "hand_rSocket");
	Backpack = CreateMeshPart<UStaticMeshComponent>(TEXT("Backpack"), "Backpack");
}

// Called when the game starts or when spawned
void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	for (EPartType pt = EPartType::PT_Cloth; pt < EPartType::PT_Backpack;)
	{
		SelectedPart.Add(pt, TEXT("NONE"));
		uint8 tmp = (uint8)pt;
		tmp++;
		pt = (EPartType)tmp;
	}

	USkeletalMeshComponent* master = this->GetMesh();
	CharacterMeshComponents.Add(TEXT("Cloth"), master);
	TArray<USceneComponent*> children;
	master->GetChildrenComponents(false, children);
	for (int i = 0; i < children.Num(); i++)
	{
		USceneComponent* comp = children[i];
		FString name = UKismetSystemLibrary::GetObjectName(comp);
		UMeshComponent* mesh = Cast<UMeshComponent>(comp);
		if (mesh)
		{
			CharacterMeshComponents.Add(name, mesh);
		}
	}
	
	for (auto& it : CharacterMeshComponents)
	{
		USkeletalMeshComponent* skeletal = Cast<USkeletalMeshComponent>(it.Value);
		if (skeletal != nullptr)
		{
			skeletal->SetMasterPoseComponent(master);
		}
	}
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

