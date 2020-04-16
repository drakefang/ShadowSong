// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Animation/AnimInstance.h"
#include "AbilitySystemInterface.h"
#include "SSTypes.h"
#include "SSCharacterBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class SHADOWSONG_API ASSCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseParts")
	TArray<FMeshPart> Parts;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Change Avatar")
	TMap<EPartType, FString> SelectedPart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Change Avatar")
	TSubclassOf<class UAnimInstance> AnimClass;

	FRotator LastVelocityRotation;
	FRotator TargetRotation;
	FRotator LastMovementInputRotation;
	bool IsMoving;
	bool HasMovementInput;
	FVector Acceleration;
	FVector PreviousVelocity;
	float Speed;
	float MovementInputAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Look")
	ERotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebugTrace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Look")
	float LookRotRate = 1.25f;

	UPROPERTY(VisibleAnywhere, Category = "Ability")
	class USSAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class USSAttributeSet* AttributeSet;
	UPROPERTY()
	uint32 bAbilitiesInitialized : 1;

public:
	// Sets default values for this character's properties
	ASSCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	template <class T>
	T* CreateMeshPart(FName name, FName socket = NAME_None)
	{
		auto part = CreateDefaultSubobject<T>(name);
		part->SetupAttachment(this->GetMesh(), socket);
		//CharacterMeshComponents.Add(name.ToString(), part);
		Parts.Add(FMeshPart{part, name, socket});
		return part;
	}

	void SetEssentialValues();
	void UpdateGroundedRotation();
	bool CanUpdateMovingRotation() const;
	void SmoothCharacterRotation(const FRotator& Target, float TargetInterpSpeed, float ActorInterpSpeed);

	//UFUNCTION(BlueprintCallable, Category = "DebugDraw")
	void DrawRealtimeVelocityArrow(FLinearColor Color = FLinearColor(1, 0, 1));
	//UFUNCTION(BlueprintCallable, Category = "DebugDraw")
	void DrawRealtimeAccelerateArrow(FLinearColor Color = FLinearColor(1, 0.5f, 0));
	//UFUNCTION(BlueprintCallable, Category = "DebugDraw")
	void DrawRealtimeCharacterRotArrow(FLinearColor Color = FLinearColor(0, 0.333f, 1));
	void DrawRealtimeControllerRotArrow(FLinearColor Color = FLinearColor(0, 1, 1));

	void FixDiagonalGamePadValues(float XIn, float YIn, float& XOut, float& YOut) const;
	void GetControlForwardRightVector(FVector& Forward, FVector& Right) const;

	void LockMouseInCenter();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement Input")
	void MovementInput(bool IsForward);
	UFUNCTION(BlueprintCallable, Category = "Camera Input")
	void CameraControlInput(float AxisValue, bool IsPitch);

	UFUNCTION(BlueprintCallable, Category = "Change Avatar")
	void AttachWeapon();
};
