// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractableInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

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
	APawn* OwnerPawn = GetOwnerPawn();
	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PC)
	{
		CurrentFocusedActor = nullptr;
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * InteractionDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, CamLoc, TraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionRadius), QueryParams
	);

	AActor* HitActor = (bHit && Hit.GetActor()) ? Hit.GetActor() : nullptr;


	if (HitActor && HitActor->Implements<UInteractableInterface>())
	{
		CurrentFocusedActor = HitActor;
	}
	else
	{
		CurrentFocusedActor = nullptr;
	}
}

void UInteractionComponent::OnInteractPressed()
{
	AActor* Focused = CurrentFocusedActor.Get();
	if (!Focused)
	{
		return;
	}

	IInteractableInterface::Execute_Interact(Focused, GetOwner());
}