// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "SSItemBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class SHADOWSONG_API USSItemBase : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:
    USSItemBase()
        : Price(0)
        , MaxCount(1)
        , MaxLevel(1)
        , AbilityLevel(1)
    {

    }

    UPROPERTY(BlueprintReadOnly, Category = Item)
    FPrimaryAssetType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    FSlateBrush ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
    int32 MaxCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
    int32 MaxLevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
    int32 AbilityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
    TSubclassOf<class UGameplayAbility> GrantedAbility;

    UFUNCTION(BlueprintCallable, Category = Item)
    FString GetIdentifierString() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
    bool IsConsumable() const;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
