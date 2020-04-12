// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SSTypes.h"
#include "SSHelper.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API USSHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static int32 startid;
	static TArray<FMeshPartRow> PartConfigs;
	static TSubclassOf<class UAnimInstance> AnimClass;
	
public:
	UFUNCTION(BlueprintCallable)
	static int32 GenerateUUID()
	{
		return startid++;
	}

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Load Config Part Table")
	static void LoadPartTable(TAssetPtr<UDataTable> Table);

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Get Config Part Table")
	static const TArray<FMeshPartRow>& GetPartConfigs();

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Load AnimClass")
	static void LoadAnimClass(TSubclassOf<class UAnimInstance> animClass);

	UFUNCTION(BlueprintCallable, Category = "GameConfig|Get AnimClass")
	static TSubclassOf<class UAnimInstance> GetAnimClass();
};
