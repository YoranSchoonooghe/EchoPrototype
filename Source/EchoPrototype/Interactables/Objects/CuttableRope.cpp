// Fill out your copyright notice in the Description page of Project Settings.

#include "CuttableRope.h"
#include "Components/StaticMeshComponent.h"
#include "../../Combat/HealthComponent.h"

ACuttableRope::ACuttableRope()
{
	RopeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RopeMesh"));
	SetRootComponent(RopeMesh);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RopeMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RopeMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ACuttableRope::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnDeath.AddDynamic(this, &ACuttableRope::HandleRopeHealthDepleted);
}

void ACuttableRope::HandleRopeHealthDepleted()
{
	RopeMesh->SetVisibility(false);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnRopeSevered();
	OnRopeCut.Broadcast();
}
