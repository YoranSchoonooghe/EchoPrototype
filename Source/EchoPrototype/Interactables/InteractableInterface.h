// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECHOPROTOTYPE_API IInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool RequiresEchoVision();
};
