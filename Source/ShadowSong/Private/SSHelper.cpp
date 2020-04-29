// Fill out your copyright notice in the Description page of Project Settings.


#include "SSHelper.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StreamableRenderAsset.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"

int32 USSHelper::startid = 1000;
TSubclassOf<class UAnimInstance> USSHelper::AnimClass;
TArray<FSkeletalPartRow> USSHelper::DefaultParts;
TArray<FWeaponSocketRow> USSHelper::WeaponSockets;

void USSHelper::LoadPartTable(TAssetPtr<UDataTable> TableAsset)
{
	DefaultParts.Empty();
	UDataTable* Table = Cast<UDataTable>(UKismetSystemLibrary::LoadAsset_Blocking(TableAsset));
	for (auto& it : Table->GetRowMap())
	{
		FSkeletalPartRow* Row = (FSkeletalPartRow*)(it.Value);
		if (Row->DefaultMesh.IsNull())
		{
			UKismetSystemLibrary::LoadAsset_Blocking(Row->DefaultMesh);
		}
		DefaultParts.Add(*Row);
	}
}

void USSHelper::LoadWeaponSocketTable(TAssetPtr<UDataTable> TableAsset)
{
	DefaultParts.Empty();
	UDataTable* Table = Cast<UDataTable>(UKismetSystemLibrary::LoadAsset_Blocking(TableAsset));
	for (auto& it : Table->GetRowMap())
	{
		FWeaponSocketRow* Row = (FWeaponSocketRow*)(it.Value);
		WeaponSockets.Add(*Row);
	}
}

const TArray<FSkeletalPartRow>& USSHelper::GetDefaultParts()
{
	if (DefaultParts.Num() == 0)
	{
		static TAssetPtr<UDataTable> Ptr(FString("DataTable'/Game/Configs/DefaultParts.DefaultParts'"));
		LoadPartTable(Ptr);
	}
	return DefaultParts;
}

FName USSHelper::GetWeaponSockets(EPartType WeaponType)
{
	if (WeaponSockets.Num() == 0)
	{
		static TAssetPtr<UDataTable> Ptr(FString("DataTable'/Game/Configs/WeaponSockets.WeaponSockets'"));
		LoadWeaponSocketTable(Ptr);
	}
	for(int i = 0; i < WeaponSockets.Num(); i++)
	{
		const FWeaponSocketRow& Row = WeaponSockets[i];
		if (Row.Part == WeaponType)
		{
			return Row.Socket;
		}
	}
	return FName();
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
