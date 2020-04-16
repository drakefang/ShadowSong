// Fill out your copyright notice in the Description page of Project Settings.


#include "SSItemBase.h"

FString USSItemBase::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

bool USSItemBase::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}
	return false;
}

FPrimaryAssetId USSItemBase::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, GetFName());
}
