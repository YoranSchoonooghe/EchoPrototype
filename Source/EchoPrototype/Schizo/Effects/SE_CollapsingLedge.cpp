#include "SE_CollapsingLedge.h"

#include "EchoPrototype/Movement/ClimbingComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void USE_CollapsingLedge::Execute(AActor* OwningActor)
{
	_bIsArmed = true;
}

void USE_CollapsingLedge::Update(AActor* OwningActor, float DeltaTime)
{
	if (!_bIsArmed || _bIsCollapsing || !OwningActor)
	{
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(OwningActor, 0);
	UClimbingComponent* ClimbingComp = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UClimbingComponent>() : nullptr;
	if (!ClimbingComp || ClimbingComp->GetGrabbedLedgeActor() != OwningActor)
	{
		return;
	}

	_bIsCollapsing = true;
	CachedOwningActor = OwningActor;

	OwningActor->GetWorld()->GetTimerManager().SetTimer(CollapseTimerHandle, this, &USE_CollapsingLedge::Collapse, CollapseDelay, false);
}

void USE_CollapsingLedge::Collapse()
{
	AActor* OwningActor = CachedOwningActor.Get();
	if (!OwningActor)
	{
		return;
	}

	if (bHideActor)
	{
		OwningActor->SetActorHiddenInGame(true);
	}

	if (bDisableCollision)
	{
		OwningActor->SetActorEnableCollision(false);
	}
}
