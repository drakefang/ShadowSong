// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SSPlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSONG_API ASSPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ref")
	class ASSCharacterBase* HeroRef;
};
