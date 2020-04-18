// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SSAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API ASSAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category = "Ref")
	class ASSCharacterBase* HeroRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category = "Ref")
	class ASSPlayerControllerBase* PlayerControllerRef;
};
