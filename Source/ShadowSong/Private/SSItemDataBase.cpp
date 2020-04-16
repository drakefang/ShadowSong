// Fill out your copyright notice in the Description page of Project Settings.


#include "SSItemDataBase.h"

FString USSItemDataBase::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

bool USSItemDataBase::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}
	return false;
}

FPrimaryAssetId USSItemDataBase::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, GetFName());
}
