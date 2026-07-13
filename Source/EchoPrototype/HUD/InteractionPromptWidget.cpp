// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionPromptWidget.h"
#include "../Interactables/InteractionComponent.h"
#include "Components/TextBlock.h"

void UInteractionPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UInteractionPromptWidget::NativeDestruct()
{
	if (BoundInteractionComponent.IsValid())
	{
		BoundInteractionComponent->OnFocusedActorChanged.RemoveDynamic(this, &UInteractionPromptWidget::HandleFocusedActorChanged);
	}

	Super::NativeDestruct();
}

void UInteractionPromptWidget::InitializeForInteractionComponent(UInteractionComponent* InInteractionComponent)
{
	if (!InInteractionComponent)
	{
		return;
	}

	if (BoundInteractionComponent.IsValid())
	{
		BoundInteractionComponent->OnFocusedActorChanged.RemoveDynamic(this, &UInteractionPromptWidget::HandleFocusedActorChanged);
	}

	BoundInteractionComponent = InInteractionComponent;
	InInteractionComponent->OnFocusedActorChanged.AddDynamic(this, &UInteractionPromptWidget::HandleFocusedActorChanged);
}

void UInteractionPromptWidget::HandleFocusedActorChanged(AActor* NewFocusedActor, FText InteractionPrompt)
{
	if (NewFocusedActor && !InteractionPrompt.IsEmpty())
	{
		if (PromptText)
		{
			PromptText->SetText(InteractionPrompt);
		}
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
