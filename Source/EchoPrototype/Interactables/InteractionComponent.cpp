// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractableInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "../Character/EchoComponent.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateFocus();
}

APawn* UInteractionComponent::GetOwnerPawn() const
{
	return Cast<APawn>(GetOwner());
}

void UInteractionComponent::UpdateFocus()
{
	AActor* PreviousFocusedActor = CurrentFocusedActor.Get();

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	APawn* PossessedPawn = PC ? PC->GetPawn() : nullptr;

	if (!PC || !PossessedPawn)
	{
		CurrentFocusedActor = nullptr;
	}
	else
	{
		FVector CamLoc;
		FRotator CamRot;
		
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		const bool bIsPossessingEcho = PossessedPawn->IsA<AEchoCharacter>();

		const FVector TraceEnd = CamLoc + (CamRot.Vector() * InteractionDistance);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PossessedPawn);

		FHitResult Hit;
		const bool bHit = GetWorld()->SweepSingleByChannel(
			Hit, CamLoc, TraceEnd, FQuat::Identity, ECC_Visibility,
			FCollisionShape::MakeSphere(InteractionRadius), QueryParams
		);

		AActor* HitActor = (bHit && Hit.GetActor()) ? Hit.GetActor() : nullptr;

		if (HitActor && HitActor->Implements<UInteractableInterface>())
		{
			const bool bRequiresEcho = IInteractableInterface::Execute_RequiresEchoVision(HitActor);

			CurrentFocusedActor = (bRequiresEcho && !bIsPossessingEcho) ? nullptr : HitActor;
		}
		else
		{
			CurrentFocusedActor = nullptr;
		}

		AActor* NewFocusedActor = CurrentFocusedActor.Get();

		if (NewFocusedActor != PreviousFocusedActor)
		{
			FText Prompt = FText::GetEmpty();
			if (NewFocusedActor && NewFocusedActor->Implements<UInteractableInterface>())
			{
				Prompt = IInteractableInterface::Execute_GetInteractionPrompt(NewFocusedActor);
			}

			OnFocusedActorChanged.Broadcast(NewFocusedActor, Prompt);
		}
	}
}

void UInteractionComponent::OnInteractPressed()
{
	AActor* Focused = CurrentFocusedActor.Get();
	if (!Focused)
	{
		return;
	}

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	AActor* Interactor = (PC && PC->GetPawn()) ? static_cast<AActor*>(PC->GetPawn()) : GetOwner();

	IInteractableInterface::Execute_Interact(Focused, Interactor);
}