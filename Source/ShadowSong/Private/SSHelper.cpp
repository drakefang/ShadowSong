// Fill out your copyright notice in the Description page of Project Settings.


#include "SSHelper.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StreamableRenderAsset.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"

int32 USSHelper::startid = 1000;
TArray<FMeshPartRow> USSHelper::PartConfigs;
TSubclassOf<class UAnimInstance> USSHelper::AnimClass;

void USSHelper::LoadPartTable(TAssetPtr<UDataTable> TableAsset)
{
	PartConfigs.Empty();
	UDataTable* Table = Cast<UDataTable>(UKismetSystemLibrary::LoadAsset_Blocking(TableAsset));
	for (auto& it : Table->GetRowMap())
	{
		FMeshPartRow* Row = (FMeshPartRow*)(it.Value);
		if (Row->DefaultMesh.IsNull())
		{
			UKismetSystemLibrary::LoadAsset_Blocking(Row->DefaultMesh);
		}
		PartConfigs.Add(*Row);
	}
}

const TArray<FMeshPartRow>& USSHelper::GetPartConfigs()
{
	if (PartConfigs.Num() == 0)
	{
		static TAssetPtr<UDataTable> Ptr(FString("DataTable'/Game/Configs/CharacterPartConfig.CharacterPartConfig'"));
		LoadPartTable(Ptr);
	}
	return PartConfigs;
}

void USSHelper::LoadAnimClass(TSubclassOf<class UAnimInstance> animClass)
{
	AnimClass = animClass;
}

TSubclassOf<class UAnimInstance> USSHelper::GetAnimClass()
{
	if (!AnimClass.Get())
	{
		AnimClass = LoadClass<UAnimInstance>(nullptr, TEXT("AnimBlueprint'/Game/Characters/ABP_Player.ABP_Player_C'"));
	}
	return AnimClass;
}

AActor* USSHelper::EffectContextGetInstigatorActor(FGameplayEffectSpec const& Spec)
{
	return UAbilitySystemBlueprintLibrary::EffectContextGetInstigatorActor(Spec.GetEffectContext());
}
