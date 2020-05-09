// Fill out your copyright notice in the Description page of Project Settings.


#include "SSGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "SSAbilitySystemComponent.h"
#include "SSCharacterBase.h"
#include "SSTargetType.h"

bool FSSGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetEffectSpecs.Num() > 0;
}

bool FSSGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetData.Num() > 0;
}

void FSSGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults,
	const TArray<AActor*>& TargetActors)
{
	for (const auto& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

USSGameplayAbility::USSGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void USSGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

FSSGameplayEffectContainerSpec USSGameplayAbility::MakeEffectContainerSpecFromContainer(
	const FSSGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSSGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ASSCharacterBase* OwningCharacter = Cast<ASSCharacterBase>(OwningActor);
	USSAbilitySystemComponent* OwningASC = USSAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);
	if (OwningASC)
	{
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const USSTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = this->GetAbilityLevel();
		}

		for(const auto& EffectClass : Container.GameplayEffects)
		{
			ReturnSpec.TargetEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}

	return ReturnSpec;
}

FSSGameplayEffectContainerSpec USSGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag,
	const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSSGameplayEffectContainer* FoundContainer = EffectMap.Find(ContainerTag);
	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FSSGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> USSGameplayAbility::ApplyEffectContainerSpec(
	const FSSGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	for(const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> USSGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag,
	const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FSSGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}
