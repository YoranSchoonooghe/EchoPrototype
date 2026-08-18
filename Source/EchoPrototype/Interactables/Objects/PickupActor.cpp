// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"

#include "../../SaveGame/SaveGameSubsystem.h"

void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsAlreadyCollected())
	{
		Destroy();
	}
}

void APickupActor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	OnPickedUp(Interactor);
	MarkCollected();
	Destroy();
}

bool APickupActor::IsAlreadyCollected() const
{
	if (USaveGameSubsystem* SaveSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		return SaveSubsystem->IsPickupCollected(GetFName());
	}

	return false;
}

void APickupActor::MarkCollected() const
{
	if (USaveGameSubsystem* SaveSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		SaveSubsystem->MarkPickupCollected(GetFName());
	}
}
