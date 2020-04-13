// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SSPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API ASSPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	ASSPlayerControllerBase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
protected:
	class ASSCharacterBase* HeroRef;

protected:
	UFUNCTION()
	void InitPlayer();

	FTransform GetSpawnRandomTransform() const;
};
