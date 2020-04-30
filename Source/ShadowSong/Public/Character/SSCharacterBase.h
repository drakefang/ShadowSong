// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowSong/ShadowSong.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "SSTypes.h"
#include "SSCharacterBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class SHADOWSONG_API ASSCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class USSGameplayAbility>> GameplayAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	class USSAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Abilities")
	int32 CharacterLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	TMap<ESSAbilityID, FGameplayAbilitySpecHandle> AbilityHandlesMap;

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
	uint32 bShowDebugTrace : 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Look")
	float LookRotRate;
	UPROPERTY(Replicated)
	FRotator ControlRotation;

	UPROPERTY()
	class USSAttributeSet* AttributeSet;

	UPROPERTY()
	uint32 bAbilitiesInitialized : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Anim")
	EWeaponAnimType WeaponAnimType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseParts")
	TArray<FMeshPart> Parts;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Avatar")
	TMap<EPartType, FString> SelectedPart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar")
	TSubclassOf<class UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class ASSWeaponBase* RightHandWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class ASSWeaponBase* LeftHandWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class ASSWeaponBase* Shield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class ASSWeaponBase* Backpack;
	
	FGameplayTag DeadTag;

public:
	// Sets default values for this character's properties
	ASSCharacterBase(const class FObjectInitializer& ObjectInitializer);

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
	void DrawRealtimeVelocityArrow(FLinearColor Color = FLinearColor(1, 0, 1)) const;
	//UFUNCTION(BlueprintCallable, Category = "DebugDraw")
	void DrawRealtimeAccelerateArrow(FLinearColor Color = FLinearColor(1, 0.5f, 0)) const;
	//UFUNCTION(BlueprintCallable, Category = "DebugDraw")
	void DrawRealtimeCharacterRotArrow(FLinearColor Color = FLinearColor(0, 0.333f, 1)) const;
	void DrawRealtimeControllerRotArrow(FLinearColor Color = FLinearColor(0, 1, 1)) const;

	static void FixDiagonalGamePadValues(float XIn, float YIn, float& XOut, float& YOut);
	void GetControlForwardRightVector(FVector& Forward, FVector& Right) const;

	void LockMouseInCenter() const;

	virtual void AddStartupGameplayAbilities();
	virtual void RemoveStartupGameplayAbilities();

	virtual void InitializeAttributes();
	virtual void AddStartupEffects();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
	virtual void SetMoveSpeed(float MoveSpeed);

	void SaveActivateAbilityHandles(const FGameplayAbilitySpec& Spec);

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement Input")
	void MovementInput(bool IsForward);
	UFUNCTION(BlueprintCallable, Category = "Camera Input")
	void CameraControlInput(float AxisValue, bool IsPitch);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTag(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true) const;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbility(ESSAbilityID ID, bool bAllowRemoteActivation = true);

public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool SetCharacterLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeedBase() const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation, Category = "Avatar")
	void AttachWeapon(class USSWeaponItem* Weapon);
	void AttachWeaponInternal(class USSWeaponItem* Weapon) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer GameplayTagContainer, TArray<class USSGameplayAbility*>& ActiveAbilities) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsUsingAbilityiesWithTags(FGameplayTagContainer GameplayTagContainer) const;
};
