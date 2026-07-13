// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoVisibilityComponent.h"

// Sets default values for this component's properties
UEchoVisibilityComponent::UEchoVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UEchoVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UPrimitiveComponent*> PrimeComps;
	Owner->GetComponents<UPrimitiveComponent>(PrimeComps);

	for (UPrimitiveComponent* Comp : PrimeComps)
	{
		if (Comp)
		{
			Comp->SetCustomDepthStencilValue(EchoStencilValue);
			Comp->SetRenderCustomDepth(true);
			Comp->SetVisibleInSceneCaptureOnly(false);
			Comp->SetCastHiddenShadow(false);

			Comp->MarkRenderStateDirty();
		}
	}
}