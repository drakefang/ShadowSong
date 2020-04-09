// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "SSTypes.h"
#include "SSCharacterBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class SHADOWSONG_API ASSCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Face;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Hair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Glove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Shoe;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Shoulder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Helm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseParts")
	class USkeletalMeshComponent* Belt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExtraParts")
	class UStaticMeshComponent* Shield;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExtraParts")
	class UStaticMeshComponent* LeftWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExtraParts")
	class UStaticMeshComponent* RightWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExtraParts")
	class UStaticMeshComponent* Backpack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Select")
	TMap<FString, class UMeshComponent*> CharacterMeshComponents;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Select")
	TMap<EPartType, FString> SelectedPart;

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
		CharacterMeshComponents.Add(name.ToString(), part);
		return part;
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
