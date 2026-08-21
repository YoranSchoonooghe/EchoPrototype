// Fill out your copyright notice in the Description page of Project Settings.

#include "Chandelier.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Lever.h"
#include "CuttableRope.h"
#include "../../Enemies/EnemyCharacter.h"
#include "../../Combat/HealthComponent.h"

AChandelier::AChandelier()
{
	PrimaryActorTick.bCanEverTick = false;

	ChandelierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChandelierMesh"));
	SetRootComponent(ChandelierMesh);
	ChandelierMesh->SetMobility(EComponentMobility::Movable);
	ChandelierMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChandelierMesh->SetSimulatePhysics(false);
	ChandelierMesh->SetNotifyRigidBodyCollision(true);

	KillZone = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZone"));
	KillZone->SetupAttachment(ChandelierMesh);
	KillZone->SetBoxExtent(FVector(120.0f, 120.0f, 60.0f));
	KillZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	KillZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	KillZone->SetGenerateOverlapEvents(true);
}

void AChandelier::BeginPlay()
{
	Super::BeginPlay();

	ChandelierMesh->OnComponentHit.AddDynamic(this, &AChandelier::OnChandelierMeshHit);
	KillZone->OnComponentBeginOverlap.AddDynamic(this, &AChandelier::OnKillZoneBeginOverlap);

	if (TriggerLever)
	{
		TriggerLever->OnLeverPulled.AddDynamic(this, &AChandelier::TriggerDrop);
	}

	if (TriggerRope)
	{
		TriggerRope->OnRopeCut.AddDynamic(this, &AChandelier::TriggerDrop);
	}
}

void AChandelier::TriggerDrop()
{
	if (ChandelierState != EChandelierState::Hanging)
	{
		return;
	}

	ChandelierState = EChandelierState::Falling;
	ChandelierMesh->SetSimulatePhysics(true);
}

void AChandelier::OnChandelierMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ChandelierState != EChandelierState::Falling)
	{
		return;
	}

	if (Hit.ImpactNormal.Z < 0.5f)
	{
		return;
	}

	ChandelierState = EChandelierState::Landed;
	KillZone->SetGenerateOverlapEvents(false);
	OnChandelierLanded();
}

void AChandelier::OnKillZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ChandelierState != EChandelierState::Falling)
	{
		return;
	}

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	if (!Enemy)
	{
		return;
	}

	if (UHealthComponent* Health = Enemy->FindComponentByClass<UHealthComponent>())
	{
		Health->Kill(this);
	}
}
