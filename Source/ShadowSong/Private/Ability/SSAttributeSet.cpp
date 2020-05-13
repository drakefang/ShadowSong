// Fill out your copyright notice in the Description page of Project Settings.


#include "SSAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "SSCharacterBase.h"

class DamagePostExecutor : public PostEffectExecutor
{
public:
	virtual void Executor(UAbilitySystemComponent* Source, const FGameplayTagContainer& SourceTags,
		const FGameplayEffectModCallbackData& Data, USSAttributeSet* Set) override
	{
		AActor* SourceActor = nullptr;
		AActor* TargetActor = nullptr;
		ASSCharacterBase* TargetCharacter = nullptr;
		ASSCharacterBase* SourceCharacter = nullptr;
		AController* TargetController = nullptr;
		AController* SourceController = nullptr;
		FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			TargetCharacter = Cast<ASSCharacterBase>(TargetActor);
			TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		}
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				APawn* Pawn = Cast<APawn>(SourceActor);
				if (Pawn)
				{
					SourceController = Pawn->GetController();
				}
			}
			if (SourceController)
			{
				SourceCharacter = Cast<ASSCharacterBase>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ASSCharacterBase>(SourceActor);
			}
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}
		const float LocalDamage = Set->GetDamage();
		Set->SetDamage(0);
		if (LocalDamage > 0)
		{
			const float OldHealth = Set->GetHealth();
			Set->SetHealth(FMath::Clamp(OldHealth - LocalDamage, 0.f, Set->GetMaxHealth()));

			if (TargetCharacter)
			{
				TargetCharacter->HandleDamage(LocalDamage, HitResult, SourceTags, SourceCharacter, SourceActor);

				TargetCharacter->HandleHealthChanged(-LocalDamage, SourceTags);
			}
		}
	}
};

class HealthPostExecutor : public PostEffectExecutor
{
public:
	virtual void Executor(UAbilitySystemComponent* Source, const FGameplayTagContainer& SourceTags,
		const FGameplayEffectModCallbackData& Data, USSAttributeSet* Set) override
	{
		Set->SetHealth(FMath::Clamp(Set->GetHealth(), 0.0f, Set->GetMaxHealth()));

		ASSCharacterBase* TargetCharacter = nullptr;
		AActor* TargetActor = nullptr;
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			TargetCharacter = Cast<ASSCharacterBase>(TargetActor);
		}
		float DeltaValue = 0;
		if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
		{
			// If this was additive, store the raw delta value to be passed along later
			DeltaValue = Data.EvaluatedData.Magnitude;
		}
		if (TargetCharacter)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
};

class ManaPostExecutor : public PostEffectExecutor
{
public:
	virtual void Executor(UAbilitySystemComponent* Source, const FGameplayTagContainer& SourceTags,
		const FGameplayEffectModCallbackData& Data, USSAttributeSet* Set) override
	{
		Set->SetMana(FMath::Clamp(Set->GetMana(), 0.0f, Set->GetMaxMana()));

		ASSCharacterBase* TargetCharacter = nullptr;
		AActor* TargetActor = nullptr;
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			TargetCharacter = Cast<ASSCharacterBase>(TargetActor);
		}
		float DeltaValue = 0;
		if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
		{
			// If this was additive, store the raw delta value to be passed along later
			DeltaValue = Data.EvaluatedData.Magnitude;
		}
		if (TargetCharacter)
		{
			TargetCharacter->HandleManaChanged(DeltaValue, SourceTags);
		}
	}
};

class MoveSpeedPostExecutor : public PostEffectExecutor
{
public:
	virtual void Executor(UAbilitySystemComponent* Source, const FGameplayTagContainer& SourceTags,
		const FGameplayEffectModCallbackData& Data, USSAttributeSet* Set) override
	{
		ASSCharacterBase* TargetCharacter = nullptr;
		AActor* TargetActor = nullptr;
		if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		{
			TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			TargetCharacter = Cast<ASSCharacterBase>(TargetActor);
		}
		float DeltaValue = 0;
		if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
		{
			// If this was additive, store the raw delta value to be passed along later
			DeltaValue = Data.EvaluatedData.Magnitude;
		}
		if (TargetCharacter)
		{
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue, SourceTags);
		}
	}
};

USSAttributeSet::USSAttributeSet() 
	: Health(1.f)
	, MaxHealth(1.f)
	, Mana(0)
	, MaxMana(0)
	, Damage(0)
	, AttackPower(1.f)
	, DefensePower(1.f)
	, Level(1)
{
	AttributeExecutors.Add(GetDamageAttribute(), new DamagePostExecutor());
	AttributeExecutors.Add(GetManaAttribute(), new ManaPostExecutor());
	AttributeExecutors.Add(GetHealthAttribute(), new HealthPostExecutor());
	AttributeExecutors.Add(GetMoveSpeedAttribute(), new MoveSpeedPostExecutor());
}

USSAttributeSet::~USSAttributeSet()
{
	for(const auto& kv : AttributeExecutors)
	{
		delete kv.Value;
	}
}

void USSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
}

void USSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FString cs = Data.EffectSpec.GetContext().ToString();
	UE_LOG(LogGame, Display, TEXT("%s:%s"), TEXT(__FUNCTION__), *cs);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	PostEffectExecutor** ExecutorPtr = AttributeExecutors.Find(Data.EvaluatedData.Attribute);
	if (ExecutorPtr)
	{
		(*ExecutorPtr)->Executor(Source, SourceTags, Data, this);
	}
}

void USSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USSAttributeSet, Health);
	DOREPLIFETIME(USSAttributeSet, MaxHealth);
	DOREPLIFETIME(USSAttributeSet, Mana);
	DOREPLIFETIME(USSAttributeSet, MaxMana);
	DOREPLIFETIME(USSAttributeSet, AttackPower);
	DOREPLIFETIME(USSAttributeSet, DefensePower);
	DOREPLIFETIME(USSAttributeSet, MoveSpeed);
	DOREPLIFETIME(USSAttributeSet, Stamina);
	DOREPLIFETIME(USSAttributeSet, MaxStamina);
}

void USSAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* Comp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && Comp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentMaxValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
		Comp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void USSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Health, OldValue);
}

void USSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxHealth, OldValue);
}

void USSAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Mana, OldValue);
}

void USSAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxMana, OldValue);
}

void USSAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MoveSpeed, OldValue);
}

void USSAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, AttackPower, OldValue);
}

void USSAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, DefensePower, OldValue);
}

void USSAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Stamina, OldValue);
}

void USSAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, MaxStamina, OldValue);
}

void USSAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USSAttributeSet, Level, OldValue);
}
