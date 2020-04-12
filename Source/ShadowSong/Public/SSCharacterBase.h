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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseParts")
	TArray<FMeshPart> Parts;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Select")
	TMap<EPartType, FString> SelectedPart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Select")
	TSubclassOf<class UAnimInstance> AnimClass;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
