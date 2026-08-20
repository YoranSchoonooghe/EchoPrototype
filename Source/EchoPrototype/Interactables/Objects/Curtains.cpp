// Fill out your copyright notice in the Description page of Project Settings.


#include "Curtains.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "../../Enemies/EnemyCharacter.h"
#include "../../Combat/HealthComponent.h"

ACurtains::ACurtains()
{
	PrimaryActorTick.bCanEverTick = true;

	CurtainsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CurtainsRoot"));
	SetRootComponent(CurtainsRoot);

	LeftCurtainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftCurtainMesh"));
	LeftCurtainMesh->SetupAttachment(CurtainsRoot);
	LeftCurtainMesh->SetMobility(EComponentMobility::Movable);

	RightCurtainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightCurtainMesh"));
	RightCurtainMesh->SetupAttachment(CurtainsRoot);
	RightCurtainMesh->SetMobility(EComponentMobility::Movable);

	KillZone = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZone"));
	KillZone->SetupAttachment(CurtainsRoot);
	KillZone->SetBoxExtent(FVector(100.0f, 75.0f, 100.0f));
	KillZone->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	KillZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	KillZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	KillZone->SetGenerateOverlapEvents(true);
}

void ACurtains::BeginPlay()
{
	Super::BeginPlay();

	const FVector RightOffset(0.0f, OpenDistance, 0.0f);

	LeftClosedLocation = LeftCurtainMesh->GetRelativeLocation();
	LeftOpenLocation = LeftClosedLocation - RightOffset;

	RightClosedLocation = RightCurtainMesh->GetRelativeLocation();
	RightOpenLocation = RightClosedLocation + RightOffset;
}

void ACurtains::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurtainsState != ECurtainsState::Opening && CurtainsState != ECurtainsState::Closing)
	{
		return;
	}

	const bool bOpening = (CurtainsState == ECurtainsState::Opening);
	const FVector LeftTarget = bOpening ? LeftOpenLocation : LeftClosedLocation;
	const FVector RightTarget = bOpening ? RightOpenLocation : RightClosedLocation;

	const FVector NewLeftLocation = FMath::VInterpConstantTo(LeftCurtainMesh->GetRelativeLocation(), LeftTarget, DeltaTime, OpenSpeed);
	const FVector NewRightLocation = FMath::VInterpConstantTo(RightCurtainMesh->GetRelativeLocation(), RightTarget, DeltaTime, OpenSpeed);

	LeftCurtainMesh->SetRelativeLocation(NewLeftLocation);
	RightCurtainMesh->SetRelativeLocation(NewRightLocation);

	if (NewLeftLocation.Equals(LeftTarget, 1.0f) && NewRightLocation.Equals(RightTarget, 1.0f))
	{
		LeftCurtainMesh->SetRelativeLocation(LeftTarget);
		RightCurtainMesh->SetRelativeLocation(RightTarget);
		CurtainsState = bOpening ? ECurtainsState::Open : ECurtainsState::Closed;
	}
}

void ACurtains::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	ToggleCurtains();
}

FText ACurtains::GetInteractionPrompt_Implementation() const
{
	return CurtainsState == ECurtainsState::Open
		? FText::FromString(TEXT("'E' to close curtains"))
		: FText::FromString(TEXT("'E' to open curtains"));
}

void ACurtains::ToggleCurtains()
{
	if (CurtainsState == ECurtainsState::Closed)
	{
		OpenCurtains();
	}
	else if (CurtainsState == ECurtainsState::Open)
	{
		CloseCurtains();
	}

}

void ACurtains::OpenCurtains()
{
	if (CurtainsState != ECurtainsState::Closed)
	{
		return;
	}

	CurtainsState = ECurtainsState::Opening;

	KillEnemiesInFront();
}

void ACurtains::CloseCurtains()
{
	if (CurtainsState != ECurtainsState::Open)
	{
		return;
	}

	CurtainsState = ECurtainsState::Closing;
}

void ACurtains::KillEnemiesInFront()
{
	if (!KillZone)
	{
		return;
	}

	TArray<AActor*> OverlappingEnemies;
	KillZone->GetOverlappingActors(OverlappingEnemies, AEnemyCharacter::StaticClass());

	for (AActor* Overlapping : OverlappingEnemies)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Overlapping);
		if (!Enemy)
		{
			continue;
		}

		if (UHealthComponent* Health = Enemy->FindComponentByClass<UHealthComponent>())
		{
			Health->Kill(this);
		}
	}
}
