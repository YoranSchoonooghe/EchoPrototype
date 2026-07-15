#include "SchizoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EchoPrototype/Character/EchoComponent.h"

USchizoComponent::USchizoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USchizoComponent::BeginPlay()
{
	Super::BeginPlay();

	BindToPlayerEchoPlaced();

	for (USchizoEffect* pSchizoEffect : SchizoEffects)
	{
		if (!pSchizoEffect) continue;

		pSchizoEffect->Initialize(GetOwner());
	}
}

void USchizoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (USchizoEffect* pSchizoEffect : SchizoEffects)
	{
		if (!pSchizoEffect) continue;

		pSchizoEffect->Update(GetOwner(), DeltaTime);
	}
}

void USchizoComponent::BindToPlayerEchoPlaced()
{
	if (ACharacter* pCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (auto* pEchoComponent = pCharacter->FindComponentByClass<UEchoComponent>())
		{
			pEchoComponent->OnPlaced.AddDynamic(this, &USchizoComponent::IncrementEchoCount);
		}
	}
}

void USchizoComponent::IncrementEchoCount()
{
	++_echoCount;

	if (_echoCount == EchoCountTrigger)
	{
		ActivateSchizoEffect();
	}
}

void USchizoComponent::ActivateSchizoEffect()
{
	for (USchizoEffect* pSchizoEffect : SchizoEffects)
	{
		if (!pSchizoEffect) continue;

		pSchizoEffect->Execute(GetOwner());
	}
	
	OnActivated.Broadcast();
}
