// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SSHelper.h"
#include "SSAbilitySystemComponent.h"
#include "SSAttributeSet.h"
#include "SSCharacterMovementComponent.h"
#include "SSGameplayAbility.h"
#include "SSPlayerControllerBase.h"
#include "SSPlayerStateBase.h"
#include "SSWeaponBase.h"
#include "SSWeaponItem.h"

const FGameplayTag ASSCharacterBase::NormalHitType = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Normal"));
const FGameplayTag ASSCharacterBase::DefendHitType = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Defend"));

// Sets default values
ASSCharacterBase::ASSCharacterBase(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)),
	CharacterLevel(1),
	LookRotRate(1.25f)
{
	AbilitySystemComponent = CreateDefaultSubobject<USSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Parts.Empty();
	TArray<FSkeletalPartRow> DefaultParts = USSHelper::GetDefaultParts();
	for (const auto& row : DefaultParts)
	{
		USkeletalMesh* Asset = row.DefaultMesh.Get();
		if (!row.DefaultMesh.IsValid())
		{
			Asset = row.DefaultMesh.LoadSynchronous();
		}
		const FName Name = *(UEnum::GetDisplayValueAsText<EPartType>(row.Part)).ToString();
		USkeletalMeshComponent* Comp = GetMesh();
		if (row.Part != EPartType::PT_Cloth)
		{
			Comp = CreateMeshPart<USkeletalMeshComponent>(Name);
			Comp->SetMasterPoseComponent(GetMesh());
		}
		else
		{
			Parts.Add(FMeshPart{ Comp, Name });
		}
		USkeletalMesh* DefaultMesh = Cast<USkeletalMesh>(Asset);
		Comp->SetSkeletalMesh(DefaultMesh);
		Comp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		//Comp->SetAnimClass(USSHelper::GetAnimClass());
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	IsMoving = false;
	HasMovementInput = false;
	RotationMode = ERotationMode::LookingDirection;
	bShowDebugTrace = false;
	ControlRotation = GetControlRotation();

	AttributeSet = CreateDefaultSubobject<USSAttributeSet>(TEXT("AttributeSet"));
	bAbilitiesInitialized = false;
	bIsArming = false;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

// Called when the game starts or when spawned
void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AnimClass == nullptr)
	{
		AnimClass = USSHelper::GetAnimClass();
	}

	for (EPartType Pt = EPartType::PT_Cloth; Pt < EPartType::PT_Backpack;)
	{
		SelectedPart.Add(Pt, TEXT("NONE"));
		uint8 Tmp = static_cast<uint8>(Pt);
		Tmp++;
		Pt = static_cast<EPartType>(Tmp);
	}

	TArray<USceneComponent*> children;
	this->GetMesh()->GetChildrenComponents(false, children);
	for (int i = 0; i < children.Num(); i++)
	{
		USkeletalMeshComponent* child = Cast<USkeletalMeshComponent>(children[i]);
		if (child == nullptr) continue;
		FString str;
		child->GetName(str);
		child->SetAnimClass(AnimClass);
		Parts.Add(FMeshPart{ child, *str, "None" });
	}

	LastVelocityRotation = GetActorRotation();
	TargetRotation = LastVelocityRotation;
	LastMovementInputRotation = LastVelocityRotation;
}

void ASSCharacterBase::SetEssentialValues()
{
	const FVector Velocity = GetVelocity();
	const FVector delta = Velocity - PreviousVelocity;
	Acceleration = delta / GetWorld()->GetDeltaSeconds();

	const FVector tmp = FVector(Velocity.X, Velocity.Y, 0);
	Speed = UKismetMathLibrary::VSize(tmp);
	IsMoving = Speed > 1.0f;

	if (IsMoving)
	{
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(Velocity);
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	const FVector ca = Movement->GetCurrentAcceleration();
	const float ma = Movement->GetMaxAcceleration();
	MovementInputAmount = UKismetMathLibrary::VSize(ca) / ma;
	HasMovementInput = MovementInputAmount > 0;

	if (HasMovementInput)
	{
		LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(ca);
	}
}

void ASSCharacterBase::UpdateGroundedRotation()
{
	if (CanUpdateMovingRotation())
	{
		switch (RotationMode)
		{
			case ERotationMode::LookingDirection:
			{
				if (GetLocalRole() == ROLE_Authority)
				{
					ControlRotation = GetControlRotation();
					ControlRotation.Roll = 0;
					ControlRotation.Pitch = 0;
				}
				SmoothCharacterRotation(ControlRotation, 500.0f, 10.0f);
				break;
			}
			case ERotationMode::VelocityDirection:
			{
				const FRotator Target = UKismetMathLibrary::MakeRotator(0, 0, LastVelocityRotation.Yaw);
				SmoothCharacterRotation(Target, 800.0f, 10.0f);
				break;
			}
		}
	}
	else
	{
		TargetRotation = GetActorRotation();
		/*switch (RotationMode)
		{
			case ERotationMode::LookingDirection:
			{
				break;
			}
			case ERotationMode::VelocityDirection:
			{
				break;
			}
		}*/
	}
}

bool ASSCharacterBase::CanUpdateMovingRotation() const
{
	bool res = IsMoving && HasMovementInput;
	res = res || Speed > 150.0f;
	res = res && !HasAnyRootMotion();
	return res;
}

void ASSCharacterBase::SmoothCharacterRotation(const FRotator& Target, float TargetInterpSpeed, 
	float ActorInterpSpeed)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, Target, 
		DeltaTime, TargetInterpSpeed);

	const FRotator NewRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, 
	                                                      DeltaTime, ActorInterpSpeed);
	SetActorRotation(NewRot);
}

// Called every frame
void ASSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetEssentialValues();

	UpdateGroundedRotation();

	if (bShowDebugTrace)
	{
		DrawRealtimeVelocityArrow();
		DrawRealtimeAccelerateArrow();
		DrawRealtimeCharacterRotArrow();
		DrawRealtimeControllerRotArrow();
	}
}

// Called to bind functionality to input
void ASSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASSCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
		AddStartupEffects();
		InitializeAttributes();

		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}

void ASSCharacterBase::UnPossessed()
{
}

void ASSCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

void ASSCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ASSPlayerStateBase* PS = GetPlayerState<ASSPlayerStateBase>();
	if (PS)
	{
		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		//BindASCInput();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		AddStartupGameplayAbilities();
		AddStartupEffects();
		InitializeAttributes();

		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());

		ASSPlayerControllerBase* PC = Cast<ASSPlayerControllerBase>(GetController());
		if (PC)
		{
			//PC->CreateHUD();
		}


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}

UAbilitySystemComponent* ASSCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASSCharacterBase::SetHealth(float Health)
{
	if (AttributeSet)
	{
		AttributeSet->SetHealth(Health);
	}
}

void ASSCharacterBase::SetMana(float Mana)
{
	if (AttributeSet)
	{
		AttributeSet->SetMana(Mana);
	}
}

void ASSCharacterBase::SetStamina(float Stamina)
{
	if (AttributeSet)
	{
		AttributeSet->SetStamina(Stamina);
	}
}

void ASSCharacterBase::SetMoveSpeed(float MoveSpeed)
{
	if (AttributeSet)
	{
		AttributeSet->SetMoveSpeed(MoveSpeed);
	}
}

void ASSCharacterBase::SaveActivateAbilityHandles(const FGameplayAbilitySpec& Spec)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	USSGameplayAbility* Ability = Cast<USSGameplayAbility>(Spec.Ability);
	AbilityHandlesMap.Add(Ability->AbilityID, Spec.Handle);
}

void ASSCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
	const FGameplayTagContainer& DamageTags, ASSCharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	this->OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ASSCharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		this->OnHealthChanged(DeltaValue, EventTags);
	}
}

void ASSCharacterBase::HandleManaChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		this->OnManaChanged(DeltaValue, EventTags);
	}
}

void ASSCharacterBase::HandleMoveSpeedChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
	if (bAbilitiesInitialized)
	{
		this->OnMoveSpeedChanged(DeltaValue, EventTags);
	}
}

void ASSCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		/*FActiveGameplayEffectHandle ActiveHandle = */AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

void ASSCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->StartupEffectApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for(const auto Effect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			/*FActiveGameplayEffectHandle ActiveHandle = */AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}
}

void ASSCharacterBase::MovementInput(bool IsForward)
{
	FVector Forward, Right;
	float XOut = 0, YOut = 0;
	GetControlForwardRightVector(Forward, Right);
	const float YIn = GetInputAxisValue("MoveForward/Backwards");
	const float XIn = GetInputAxisValue("MoveRight/Left");
	FixDiagonalGamePadValues(XIn, YIn, XOut, YOut);
	if (IsForward)
	{
		AddMovementInput(Forward, YOut);
	}
	else
	{
		AddMovementInput(Right, XOut);
	}
}

void ASSCharacterBase::CameraControlInput(float AxisValue, bool IsPitch)
{
	const float v = AxisValue * LookRotRate;
	if (IsPitch)
	{
		AddControllerPitchInput(v);
	}
	else
	{
		AddControllerYawInput(v);
	}
}

bool ASSCharacterBase::ActivateAbilitiesWithTag(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}
	return false;
}

bool ASSCharacterBase::ActivateAbility(ESSAbilityID ID, bool bAllowRemoteActivation)
{
	if (!AbilityHandlesMap.Contains(ID))
	{
		return false;
	}
	FGameplayAbilitySpecHandle* Handle = AbilityHandlesMap.Find(ID);
	if (AbilitySystemComponent && Handle)
	{
		return AbilitySystemComponent->TryActivateAbility(*Handle, bAllowRemoteActivation);
	}
	return false;
}

void ASSCharacterBase::PlayHitReact_Implementation(FGameplayTag HitType, AActor* DamageCauser)
{
	if (HitType == NormalHitType)
	{
		ShowHitReact.Broadcast(EHitReaction::HR_Get);
	}
	else if (HitType == DefendHitType)
	{
		ShowHitReact.Broadcast(EHitReaction::HR_Defend);
	}
}

bool ASSCharacterBase::PlayHitReact_Validate(FGameplayTag HitType, AActor* DamageCauser)
{
	return true;
}

int32 ASSCharacterBase::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool ASSCharacterBase::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		RemoveStartupGameplayAbilities();
		CharacterLevel = NewLevel;
		AddStartupGameplayAbilities();
		return true;
	}
	return false;
}

float ASSCharacterBase::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMana();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMaxMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxMana();
	}
	return 0.0f;
}

float ASSCharacterBase::GetStamina() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetStamina();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMaxStamina() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxStamina();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMoveSpeed() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMoveSpeed();
	}
	return 0.0f;
}

float ASSCharacterBase::GetMoveSpeedBase() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSet)->GetBaseValue();
	}

	return 0.0f;
}

void ASSCharacterBase::AttachWeaponInternal(USSWeaponItem* Weapon)
{
	ASSWeaponBase* WeaponActor;
	switch(Weapon->EquipPartType)
	{
	case EPartType::PT_RightWeapon:
		WeaponActor = RightHandWeapon;
		break;
	case EPartType::PT_LeftWeapon:
		WeaponActor = LeftHandWeapon;
		break;
	case EPartType::PT_Shield:
		WeaponActor = Shield;
		break;
	case EPartType::PT_Backpack:
		WeaponActor = Backpack;
		break;
	default:
		UE_LOG(LogGame, Display, TEXT("weapon part isn't valid!%s"), *UEnum::GetValueAsString<EPartType>(Weapon->EquipPartType));
		return;
	}

	if (IsValid(WeaponActor))
	{
		const FDetachmentTransformRules Rules(EDetachmentRule::KeepRelative,
		                                      EDetachmentRule::KeepRelative, EDetachmentRule::KeepRelative, false);
		WeaponActor->DetachFromActor(Rules);
	}
	else
	{
		WeaponActor = Cast<ASSWeaponBase>(GetWorld()->SpawnActor(Weapon->WeaponActor));
	}
	WeaponActor->AttachToComponent(GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true),
		USSHelper::GetWeaponSockets(Weapon->EquipPartType));
	WeaponActor->SetInstigator(this);

	switch (Weapon->EquipPartType)
	{
	case EPartType::PT_RightWeapon:
		RightHandWeapon = WeaponActor;
		break;
	case EPartType::PT_LeftWeapon:
		LeftHandWeapon = WeaponActor;
		break;
	case EPartType::PT_Shield:
		Shield = WeaponActor;
		break;
	case EPartType::PT_Backpack:
		Backpack = WeaponActor;
		break;
	default:
		return;
	}

	WeaponAnimType = Weapon->WeaponAnimType;
}

void ASSCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer GameplayTagContainer,
	TArray<USSGameplayAbility*>& ActiveAbilities) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);
	}
}

bool ASSCharacterBase::IsUsingAbilityiesWithTags(FGameplayTagContainer GameplayTagContainer) const
{
	TArray<USSGameplayAbility*> ActiveAbilities;
	GetActiveAbilitiesWithTags(GameplayTagContainer, ActiveAbilities);
	for(auto ability : ActiveAbilities)
	{
		UE_LOG(LogGame, Display, TEXT("%s"), *ability->AbilityTags.ToStringSimple());
	}
	//UE_LOG(LogGame, Display, TEXT("=========================="));
	return ActiveAbilities.Num() > 0;
}

bool ASSCharacterBase::AttachWeapon_Validate(class USSWeaponItem* Weapon)
{
	return true;
}

void ASSCharacterBase::AttachWeapon_Implementation(class USSWeaponItem* Weapon)
{
	AttachWeaponInternal(Weapon);
}

void ASSCharacterBase::DrawRealtimeVelocityArrow(FLinearColor Color) const
{
	const FVector Location = GetActorLocation();
	const FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(LastVelocityRotation);
	const FVector V = GetVelocity();
	if (!UKismetMathLibrary::EqualEqual_VectorVector(V, FVector::ZeroVector))
	{
		LineEnd = V;
	}
	else
	{
		Color = Color * 0.25f;
	}
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd);
	const float Len = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(V), 
	                                                      0, GetCharacterMovement()->MaxWalkSpeed, 50.0f, 75.0f);
	LineEnd *= Len;
	LineEnd += LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 60.0f, Color, 0, 5.0f);
}

void ASSCharacterBase::DrawRealtimeAccelerateArrow(FLinearColor Color) const
{
	const FVector Location = GetActorLocation();
	const FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 3.5f);
	const FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(LastMovementInputRotation);
	const FVector Acc = GetCharacterMovement()->GetCurrentAcceleration();
	if (!UKismetMathLibrary::EqualEqual_VectorVector(Acc, FVector::ZeroVector))
	{
		LineEnd = Acc;
	}
	else
	{
		Color = Color * 0.25f;
	}
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd);
	float Len = UKismetMathLibrary::VSize(Acc);
	Len = Len / GetCharacterMovement()->GetMaxAcceleration();
	Len = UKismetMathLibrary::MapRangeClamped(Len, 0, 1, 50, 75);
	LineEnd *= Len;
	LineEnd += LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 50.0f, Color, 0, 3);
}

void ASSCharacterBase::DrawRealtimeCharacterRotArrow(const FLinearColor Color) const
{
	const FVector Location = GetActorLocation();
	const FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 7);
	const FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(TargetRotation);
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd) * 50 + LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 50.0f, Color, 0, 3);
}

void ASSCharacterBase::DrawRealtimeControllerRotArrow(FLinearColor Color) const
{
	const FVector Location = GetActorLocation();
	const FVector Offset = FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector LineStart = Location - Offset;

	FRotator rot = GetControlRotation();
	rot.Pitch = 0;
	rot.Roll = 0;
	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(rot);
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd) * 100 + LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 50.0f, Color, 0, 3);
}

void ASSCharacterBase::FixDiagonalGamePadValues(float XIn, float YIn, float& XOut, float& YOut)
{
	const float ax = UKismetMathLibrary::Abs(XIn);
	const float ay = UKismetMathLibrary::Abs(YIn);
	YOut = UKismetMathLibrary::MapRangeClamped(ax, 0, 0.6f, 1.0f, 1.2f) * YIn;
	YOut = UKismetMathLibrary::FClamp(YOut, -1, 1);

	XOut = UKismetMathLibrary::MapRangeClamped(ay, 0, 0.6f, 1.0f, 1.2f) * XIn;
	XOut = UKismetMathLibrary::FClamp(XOut, -1, 1);
}

void ASSCharacterBase::GetControlForwardRightVector(FVector& Forward, FVector& Right) const
{
	const FRotator rot = this->GetControlRotation();
	Forward = UKismetMathLibrary::GetForwardVector(FRotator(0, rot.Yaw, 0));
	Right = UKismetMathLibrary::GetRightVector(FRotator(0, rot.Yaw, 0));
}

void ASSCharacterBase::LockMouseInCenter() const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || PC->bShowMouseCursor)
	{
		return;
	}
	int x = 0, y = 0;
	PC->GetViewportSize(x, y);
	PC->SetMouseLocation(x / 2, y / 2);
}

void ASSCharacterBase::AddStartupGameplayAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->StartupAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<USSGameplayAbility>& Ability : GameplayAbilities)
	{
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(*Ability, GetCharacterLevel(), 
			INDEX_NONE, this));
		AbilityHandlesMap.Add(Ability.GetDefaultObject()->AbilityID, Handle);
	}

	bAbilitiesInitialized = true;
	AbilitySystemComponent->StartupAbilitiesGiven = true;
}

void ASSCharacterBase::RemoveStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && bAbilitiesInitialized)
	{
		TArray<FGameplayAbilitySpecHandle> AbilitiesRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (Spec.SourceObject == this && GameplayAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesRemove.Add(Spec.Handle);
			}
		}

		for (int32 i = 0; i < AbilitiesRemove.Num(); i++)
		{
			AbilitySystemComponent->ClearAbility(AbilitiesRemove[i]);
		}

		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);
		bAbilitiesInitialized = false;
		AbilitySystemComponent->StartupAbilitiesGiven = false;
	}
}

void ASSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSCharacterBase, CharacterLevel);
	DOREPLIFETIME(ASSCharacterBase, ControlRotation);
	DOREPLIFETIME(ASSCharacterBase, WeaponAnimType);
}