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

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Ref")
	class ASSPawnBase* ClientPawnRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Ref")
	class ASSCharacterBase* HeroRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ref")
	class ASSAIControllerBase* ServerControllerRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref")
	TSubclassOf<class ASSCharacterBase> HeroClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ref")
	TSubclassOf<class ASSAIControllerBase> ServerControllerClass;

protected:
	UFUNCTION()
	void InitPlayer();

	FTransform GetSpawnTransform() const;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Server")
	void InitPlayerOnServer();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Server")
	void SpawnPlayerOnServer();
};
