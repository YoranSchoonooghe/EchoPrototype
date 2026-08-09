// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "PickupActor.generated.h"

// Base class for permanently collected pickups, skill points ... maybe more things
UCLASS(Abstract)
class ECHOPROTOTYPE_API APickupActor : public AInteractableActor
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnPickedUp(AActor* Interactor) {};

private:
	bool IsAlreadyCollected() const;
	void MarkCollected() const;
};
