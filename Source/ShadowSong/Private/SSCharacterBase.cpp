// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SSHelper.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TArray<FMeshPartRow> parts = USSHelper::GetPartConfigs();
	for (const auto& row : parts)
	{
		UStreamableRenderAsset* Asset = row.DefaultMesh.Get();
		if (!row.DefaultMesh.IsValid())
		{
			Asset = row.DefaultMesh.LoadSynchronous();
		}
		if (row.Socket == "None")
		{
			USkeletalMeshComponent* Comp = GetMesh();
			if (row.Name != "Cloth")
			{
				Comp = CreateMeshPart<USkeletalMeshComponent>(row.Name);
				Comp->SetMasterPoseComponent(GetMesh());
			}
			else
			{
				Parts.Add(FMeshPart{Comp, row.Name, row.Socket});
			}
			USkeletalMesh* DefaultMesh = Cast<USkeletalMesh>(Asset);
			Comp->SetSkeletalMesh(DefaultMesh);
			Comp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			Comp->SetAnimClass(USSHelper::GetAnimClass());
		}
		else
		{
			auto Comp = CreateMeshPart<UStaticMeshComponent>(row.Name, row.Socket);
			UStaticMesh* DefaultMesh = Cast<UStaticMesh>(Asset);
			Comp->SetStaticMesh(DefaultMesh);
		}
	}
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

