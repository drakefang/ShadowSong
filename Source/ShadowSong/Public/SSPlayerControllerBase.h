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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
	FVector ClickLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	float MinClickDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	float LocationInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	float RotationInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	bool EnableRotationInterp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	TSubclassOf<AActor> ClickClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	float ClickLife;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
	float RealTimeSpeed;

protected:
	UFUNCTION()
	void InitPlayer();

	FTransform GetSpawnTransform() const;
	FRotator GetMoveRotator() const;

	UFUNCTION(Server, Reliable, WithValidation, Category = "Server")
	void InitPlayerOnServer();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Server")
	void SpawnPlayerOnServer();
	UFUNCTION(Server, Reliable, WithValidation, Category = "Server")
	void UpdatePlayerOnServer(FVector Click);
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Server")
	void MoveForwardOnServer(float ScaleValue, float DeltaTime, float Speed);
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Server")
	void MoveRightOnServer(float ScaleValue, float DeltaTime, float Speed);

	UFUNCTION(BlueprintCallable, Category = "Client")
	void SetDestination(bool Pressed);
	UFUNCTION(BlueprintCallable, Category = "Client")
	bool OnMouseClick();
	UFUNCTION(BlueprintCallable, Category = "Client")
	void UpdatePlayer(float DeltaTime);
};
