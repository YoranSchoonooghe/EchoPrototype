#include "SE_ToggleVisibility.h"

void USE_ToggleVisibility::Initialize(AActor* OwningActor)
{
	_bIsVisible = !bStartHidden;

	UpdateVisibility(OwningActor);
}

void USE_ToggleVisibility::Execute(AActor* OwningActor)
{
	_bIsVisible = !_bIsVisible;

	UpdateVisibility(OwningActor);
}

void USE_ToggleVisibility::UpdateVisibility(AActor* OwningActor)
{
	OwningActor->SetActorHiddenInGame(_bIsVisible);
	if (bUseCollision)
		OwningActor->SetActorEnableCollision(!_bIsVisible);
}
