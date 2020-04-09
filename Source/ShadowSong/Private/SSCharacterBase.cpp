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

	Face = CreateMeshPart<USkeletalMeshComponent>("Face");
	Hair = CreateMeshPart<USkeletalMeshComponent>("Hair");
	Glove = CreateMeshPart<USkeletalMeshComponent>("Glove");
	Shoe = CreateMeshPart<USkeletalMeshComponent>("Shoe");
	Shoulder = CreateMeshPart<USkeletalMeshComponent>("Shoulder");
	Helm = CreateMeshPart<USkeletalMeshComponent>("Helm");
	Belt = CreateMeshPart<USkeletalMeshComponent>("Belt");

	Shield = CreateMeshPart<UStaticMeshComponent>("Shield", "hand_l");
	LeftWeapon = CreateMeshPart<UStaticMeshComponent>("LeftWeapon", "hand_lSocket");
	RightWeapon = CreateMeshPart<UStaticMeshComponent>("RightWeapon", "hand_rSocket");
	Backpack = CreateMeshPart<UStaticMeshComponent>("Backpack", "Backpack");
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
	CharacterMeshComponents.Add(TEXT("Cloth"), master);
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

