// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SSTypes.h"
#include "GameplayEffect.h"
#include "SSHelper.generated.h"

struct FGameplayEffectSpec;
class ASSCharacterBase;
/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static int32 startid;
	static TSubclassOf<class UAnimInstance> AnimClass;
	static TArray<FSkeletalPartRow> DefaultParts;
	static TArray<FWeaponSocketRow> WeaponSockets;
	
public:
	UFUNCTION(BlueprintCallable)
	static int32 GenerateUUID()
	{
		return startid++;
	}

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Load Config Part Table")
	static void LoadPartTable(TAssetPtr<UDataTable> Table);

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Load Weapon Socket Table")
	static void LoadWeaponSocketTable(TAssetPtr<UDataTable> TableAsset);
	
	UFUNCTION(BlueprintCallable, Category = "GameConfig|Get Config Part Table")
	static const TArray<FSkeletalPartRow>& GetDefaultParts();
	
	UFUNCTION(BlueprintCallable, Category = "GameConfig|Get Weapon Socket")
	static FName GetWeaponSockets(EPartType WeaponType);

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Load AnimClass")
	static void LoadAnimClass(TSubclassOf<class UAnimInstance> animClass);

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Get AnimClass")
	static TSubclassOf<class UAnimInstance> GetAnimClass();

	UFUNCTION(BlueprintCallable, Category = "Ability|Effect|Actor", Meta = (DisplayName = "GetInstigatorActor"))
	static AActor* EffectContextGetInstigatorActor(FGameplayEffectSpec const& Spec);
};
