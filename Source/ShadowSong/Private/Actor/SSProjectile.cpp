// Fill out your copyright notice in the Description page of Project Settings.


#include "SSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASSProjectile::ASSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0;
	ProjectileMovement->InitialSpeed = 3000.0f;

	bReplicates = true;

	NetUpdateFrequency = 100.0f;
}
