// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "Components/StaticMeshComponent.h"
#include "../../Echo/EchoCharacter.h"
#include "../../Combat/HealthComponent.h"
#include "HealthPickup.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAvailable)
	{
		AddActorLocalRotation(FRotator(0.0f, SpinSpeed * DeltaTime, 0.0f));
	}
}

void AHealthPickup::Interact_Implementation(AActor* Interactor)
{
	if (!bIsAvailable || !Interactor)
	{
		return;
	}

	if (!bUsableByEcho && Interactor->IsA<AEchoCharacter>())
	{
		return;
	}

	UHealthComponent* HealthComp = Interactor->FindComponentByClass<UHealthComponent>();
	if (!HealthComp)
	{
		return;
	}

	if (HealthComp->GetHealthPercent() >= 1.0f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Already at full health"));
		}
		return;
	}

	if (bFullHeal)
	{
		HealthComp->Heal(HealthComp->GetMaxHealth());
	}
	else
	{
		HealthComp->Heal(HealAmount);
	}

	SetAvailable(false);

	if (RespawnDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AHealthPickup::Respawn, RespawnDelay, false);
	}
	else
	{
		Destroy();
	}
}

FText AHealthPickup::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(TEXT("Press 'E' to use Health Pack"));
}

void AHealthPickup::SetAvailable(bool bAvailable)
{
	bIsAvailable = bAvailable;
	SetActorHiddenInGame(!bAvailable);
	SetActorEnableCollision(bAvailable);
}

void AHealthPickup::Respawn()
{
	SetAvailable(true);
}