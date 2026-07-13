// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"
#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "../../Echo/EchoActor.h"

// Sets default values
APressurePlate::APressurePlate()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	PlateMesh->SetupAttachment(Root);

	PressureZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PressureZone"));
	PressureZone->SetupAttachment(Root);

	PressureZone->SetBoxExtent(FVector(50.0f, 50.0f, 25.0f));
	PressureZone->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));
	PressureZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PressureZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	PressureZone->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	RestingRelativeLocation = PlateMesh->GetRelativeLocation();

	PressureZone->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnPressureZoneBeginOverlap);
	PressureZone->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnPressureZoneEndOverlap);
}

bool APressurePlate::IsValidTrigger(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this)
	{
		return false;
	}

	if (bOnlyEchoTriggers)
	{
		return (Cast<AEchoActor>(OtherActor) != nullptr);
	}

	return bOnlyPawnsTrigger ? (Cast<APawn>(OtherActor) != nullptr) : true;
}

void APressurePlate::OnPressureZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidTrigger(OtherActor) || bIsTransitioning)
	{
		return;
	}

	const bool bWasEmpty = OverlappingActors.IsEmpty();
	OverlappingActors.Add(OtherActor);


	if (bWasEmpty)
	{
		bIsTransitioning = true;

		PlateMesh->SetRelativeLocation(RestingRelativeLocation - FVector(0.0f, 0.0f, PressedDepth));

		if (LinkedDoor)
		{
			LinkedDoor->OpenDoor();
		}

		GetWorld()->GetTimerManager().SetTimer(ResetTransitionTimerHandle, this, &APressurePlate::ClearTransitionGate, 0.05f, false);
	}
}

void APressurePlate::OnPressureZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValidTrigger(OtherActor) || bIsTransitioning)
	{
		return;
	}

	OverlappingActors.Remove(OtherActor);


	if (OverlappingActors.IsEmpty())
	{
		bIsTransitioning = true;

		PlateMesh->SetRelativeLocation(RestingRelativeLocation);

		if (LinkedDoor)
		{
			LinkedDoor->CloseDoor();
		}

		GetWorld()->GetTimerManager().SetTimer(ResetTransitionTimerHandle, this, &APressurePlate::ClearTransitionGate, 0.05f, false);
	}
}

void APressurePlate::ClearTransitionGate()
{
	bIsTransitioning = false;
}