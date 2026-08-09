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

		//Use pawn viewing location (third person)
		FVector TraceStart = PossessedPawn->GetPawnViewLocation();
		FVector TraceEnd = TraceStart + (CamRot.Vector() * InteractionDistance);

		bool bIsLookingThroughEcho = PossessedPawn->IsA<AEchoCharacter>();
		if (!bIsLookingThroughEcho)
		{
			if (UEchoComponent* EchoComp = PossessedPawn->FindComponentByClass<UEchoComponent>())
			{
				bIsLookingThroughEcho = EchoComp->IsViewingThroughEcho();
			}
		}

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PossessedPawn);

		FHitResult Hit;

		const bool bHit = GetWorld()->SweepSingleByChannel(
			Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility,
			FCollisionShape::MakeSphere(InteractionRadius), QueryParams
		);

		//DrawDebugSphere(GetWorld(), TraceEnd, InteractionRadius, 12, bHit ? FColor::Green : FColor::Red, false, 0.0f);
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 0.0f);

		AActor* HitActor = (bHit && Hit.GetActor()) ? Hit.GetActor() : nullptr;

		if (HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interaction Hit: %s (Implements Interface: %s)"),
				*HitActor->GetName(),
				HitActor->Implements<UInteractableInterface>() ? TEXT("YES") : TEXT("NO"));
		}

		if (HitActor && HitActor->Implements<UInteractableInterface>())
		{
			const bool bRequiresEcho = IInteractableInterface::Execute_RequiresEchoVision(HitActor);

			CurrentFocusedActor = (bRequiresEcho && !bIsLookingThroughEcho) ? nullptr : HitActor;
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

	if (!IsValid(Focused))
	{
		CurrentFocusedActor = nullptr;
		OnFocusedActorChanged.Broadcast(nullptr, FText::GetEmpty());
	}
}