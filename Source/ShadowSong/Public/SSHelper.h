// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	
public:
	UFUNCTION(BlueprintCallable)
	static int32 GenerateUUID()
	{
		return startid++;
	}
};
