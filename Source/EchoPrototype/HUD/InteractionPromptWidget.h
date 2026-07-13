// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPromptWidget.generated.h"

class UTextBlock;
class UInteractionComponent;

/**
 * 
 */
UCLASS()
class ECHOPROTOTYPE_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InitializeForInteractionComponent(UInteractionComponent* InInteractionComponent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PromptText;

private:
	UFUNCTION()
	void HandleFocusedActorChanged(AActor* NewFocusedActor, FText InteractionPrompt);

	TWeakObjectPtr<UInteractionComponent> BoundInteractionComponent;
};
