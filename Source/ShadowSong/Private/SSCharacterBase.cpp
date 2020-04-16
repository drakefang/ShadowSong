// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SSHelper.h"
#include "SSAbilitySystemComponent.h"
#include "SSAttributeSet.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase() :
	CharacterLevel(1),
	LookRotRate(1.25f)
{
	AbilitySystemComponent = CreateDefaultSubobject<USSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Parts.Empty();
	TArray<FMeshPartRow> parts = USSHelper::GetPartConfigs();
	for (const auto& row : parts)
	{
		UStreamableRenderAsset* Asset = row.DefaultMesh.Get();
		if (!row.DefaultMesh.IsValid())
		{
			Asset = row.DefaultMesh.LoadSynchronous();
		}
		if (row.Socket == "None")
		{
			USkeletalMeshComponent* Comp = GetMesh();
			if (row.Name != "Cloth")
			{
				Comp = CreateMeshPart<USkeletalMeshComponent>(row.Name);
				Comp->SetMasterPoseComponent(GetMesh());
			}
			else
			{
				Parts.Add(FMeshPart{ Comp, row.Name, row.Socket });
			}
			USkeletalMesh* DefaultMesh = Cast<USkeletalMesh>(Asset);
			Comp->SetSkeletalMesh(DefaultMesh);
			Comp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			Comp->SetAnimClass(USSHelper::GetAnimClass());
		}
		else
		{
			auto Comp = CreateMeshPart<UStaticMeshComponent>(row.Name, row.Socket);
			UStaticMesh* DefaultMesh = Cast<UStaticMesh>(Asset);
			Comp->SetStaticMesh(DefaultMesh);
		}
	}

	IsMoving = false;
	HasMovementInput = false;
	RotationMode = ERotationMode::LookingDirection;
	bShowDebugTrace = false;

	AttributeSet = CreateDefaultSubobject<USSAttributeSet>(TEXT("AttributeSet"));
	bAbilitiesInitialized = false;
}

// Called when the game starts or when spawned
void ASSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	for (EPartType pt = EPartType::PT_Cloth; pt < EPartType::PT_Backpack;)
	{
		SelectedPart.Add(pt, TEXT("NONE"));
		uint8 tmp = (uint8)pt;
		tmp++;
		pt = (EPartType)tmp;
	}

	TArray<USceneComponent*> children;
	this->GetMesh()->GetChildrenComponents(false, children);
	for (int i = 0; i < children.Num(); i++)
	{
		UMeshComponent* child = Cast<UMeshComponent>(children[i]);
		FString str;
		child->GetName(str);
		Parts.Add(FMeshPart{ child, *str, "None" });
	}

	LastVelocityRotation = GetActorRotation();
	TargetRotation = LastVelocityRotation;
	LastMovementInputRotation = LastVelocityRotation;
}

void ASSCharacterBase::SetEssentialValues()
{
	FVector Velocity = GetVelocity();
	FVector delta = Velocity - PreviousVelocity;
	Acceleration = delta / GetWorld()->GetDeltaSeconds();
	
	FVector tmp = FVector(Velocity.X, Velocity.Y, 0);
	Speed = UKismetMathLibrary::VSize(tmp);
	IsMoving = Speed > 1.0f;

	if (IsMoving)
	{
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(Velocity);
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	FVector ca = Movement->GetCurrentAcceleration();
	float ma = Movement->GetMaxAcceleration();
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
				FRotator NewRot = GetControlRotation();
				NewRot.Roll = 0;
				NewRot.Pitch = 0;
				SmoothCharacterRotation(NewRot, 500.0f, 10.0f);
				break;
			}
			case ERotationMode::VelocityDirection:
			{
				FRotator Target = UKismetMathLibrary::MakeRotator(0, 0, LastVelocityRotation.Yaw);
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
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	TargetRotation = UKismetMathLibrary::RInterpTo_Constant(TargetRotation, Target, 
		DeltaTime, TargetInterpSpeed);
	
	FRotator NewRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, 
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
	}
}

void ASSCharacterBase::UnPossessed()
{
}

UAbilitySystemComponent* ASSCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASSCharacterBase::MovementInput(bool IsForward)
{
	FVector Forward, Right;
	float XOut = 0, YOut = 0;
	GetControlForwardRightVector(Forward, Right);
	float YIn = GetInputAxisValue("MoveForward/Backwards");
	float XIn = GetInputAxisValue("MoveRight/Left");
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
	float v = AxisValue * LookRotRate;
	if (IsPitch)
	{
		AddControllerPitchInput(v);
	}
	else
	{
		AddControllerYawInput(v);
	}
}

void ASSCharacterBase::AttachWeapon()
{
}

bool ASSCharacterBase::ActivateAbilitiesWithTag(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}
	return false;
}

int32 ASSCharacterBase::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool ASSCharacterBase::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		CharacterLevel = NewLevel;
	}
	return false;
}

void ASSCharacterBase::DrawRealtimeVelocityArrow(FLinearColor Color)
{
	FVector Location = GetActorLocation();
	FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(LastVelocityRotation);
	FVector V = GetVelocity();
	if (!UKismetMathLibrary::EqualEqual_VectorVector(V, FVector::ZeroVector))
	{
		LineEnd = V;
	}
	else
	{
		Color = Color * 0.25f;
	}
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd);
	float Len = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(V), 
		0, GetCharacterMovement()->MaxWalkSpeed, 50.0f, 75.0f);
	LineEnd *= Len;
	LineEnd += LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 60.0f, Color, 0, 5.0f);
}

void ASSCharacterBase::DrawRealtimeAccelerateArrow(FLinearColor Color)
{
	FVector Location = GetActorLocation();
	FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 3.5f);
	FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(LastMovementInputRotation);
	FVector Acc = GetCharacterMovement()->GetCurrentAcceleration();
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

void ASSCharacterBase::DrawRealtimeCharacterRotArrow(FLinearColor Color)
{
	FVector Location = GetActorLocation();
	FVector Offset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 7);
	FVector LineStart = Location - Offset;

	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(TargetRotation);
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd) * 50 + LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 50.0f, Color, 0, 3);
}

void ASSCharacterBase::DrawRealtimeControllerRotArrow(FLinearColor Color)
{
	FVector Location = GetActorLocation();
	FVector Offset = FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector LineStart = Location - Offset;

	FRotator rot = GetControlRotation();
	rot.Pitch = 0;
	rot.Roll = 0;
	FVector LineEnd = UKismetMathLibrary::Conv_RotatorToVector(rot);
	LineEnd = UKismetMathLibrary::Vector_NormalUnsafe(LineEnd) * 100 + LineStart;
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), LineStart, LineEnd, 50.0f, Color, 0, 3);
}

void ASSCharacterBase::FixDiagonalGamePadValues(float XIn, float YIn, float& XOut, float& YOut) const
{
	float ax = UKismetMathLibrary::Abs(XIn);
	float ay = UKismetMathLibrary::Abs(YIn);
	YOut = UKismetMathLibrary::MapRangeClamped(ax, 0, 0.6f, 1.0f, 1.2f) * YIn;
	YOut = UKismetMathLibrary::FClamp(YOut, -1, 1);

	XOut = UKismetMathLibrary::MapRangeClamped(ay, 0, 0.6f, 1.0f, 1.2f) * XIn;
	XOut = UKismetMathLibrary::FClamp(XOut, -1, 1);
}

void ASSCharacterBase::GetControlForwardRightVector(FVector& Forward, FVector& Right) const
{
	FRotator rot = this->GetControlRotation();
	Forward = UKismetMathLibrary::GetForwardVector(FRotator(0, rot.Yaw, 0));
	Right = UKismetMathLibrary::GetRightVector(FRotator(0, rot.Yaw, 0));
}

void ASSCharacterBase::LockMouseInCenter()
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

void ASSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASSCharacterBase, CharacterLevel);
}