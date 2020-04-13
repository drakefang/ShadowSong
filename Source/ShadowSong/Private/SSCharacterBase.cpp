// Fill out your copyright notice in the Description page of Project Settings.


#include "SSCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "SSHelper.h"

// Sets default values
ASSCharacterBase::ASSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
				Parts.Add(FMeshPart{Comp, row.Name, row.Socket});
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
	ShowDebugTrace = false;
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

	if (ShowDebugTrace)
	{
		DrawRealtimeVelocityArrow();
		DrawRealtimeAccelerateArrow();
		DrawRealtimeCharacterRotArrow();
	}
}

// Called to bind functionality to input
void ASSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ASSCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
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

