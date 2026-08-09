// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractableInterface.h"
#include "InteractableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractEvent);

UCLASS(Abstract)
class ECHOPROTOTYPE_API AInteractableActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	virtual void Interact_Implementation(AActor* Interactor) override { OnInteracted.Broadcast(); }
	virtual FText GetInteractionPrompt_Implementation() const override { return FText::FromString(TEXT("'E' to interact")); }

	virtual bool RequiresEchoVision_Implementation() override { return bRequiresEcho; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractEvent OnInteracted;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bRequiresEcho = false;

};
