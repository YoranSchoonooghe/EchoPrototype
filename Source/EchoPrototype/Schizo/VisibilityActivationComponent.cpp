#include "VisibilityActivationComponent.h"

UVisibilityActivationComponent::UVisibilityActivationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UVisibilityActivationComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->SetActorHiddenInGame(bStartHidden);
	if (bUseCollision)
		GetOwner()->SetActorEnableCollision(!bStartHidden);
}

void UVisibilityActivationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UVisibilityActivationComponent::Activate_Implementation()
{
	GetOwner()->SetActorHiddenInGame(!bStartHidden);
	if (bUseCollision)
		GetOwner()->SetActorEnableCollision(bStartHidden);
}
