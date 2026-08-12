// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoDistractionComponent.h"

#include "EchoCharacter.h"
#include "../Combat/HealthComponent.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"

UEchoDistractionComponent::UEchoDistractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsDetectable = true;
}

void UEchoDistractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AEchoCharacter* Echo = Cast<AEchoCharacter>(GetOwner()))
	{
		if (Echo->GetVisualState() == EEchoVisualState::Placed)
		{
			HandlePlaced();
		}
		else
		{
			Echo->OnPlaced.AddDynamic(this, &UEchoDistractionComponent::HandlePlaced);
		}
	}

	if (AActor* Owner = GetOwner())
	{
		if (UHealthComponent* Health = Owner->FindComponentByClass<UHealthComponent>())
		{
			Health->OnDeath.AddDynamic(this, &UEchoDistractionComponent::HandleDied);
		}
	}
}

void UEchoDistractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SignalTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UEchoDistractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEchoDistractionComponent::HandlePlaced()
{
	GetWorld()->GetTimerManager().SetTimer(SignalTimerHandle, this, &UEchoDistractionComponent::EmitDistractionSignal, SignalInterval, true);
}

void UEchoDistractionComponent::EmitDistractionSignal()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Owner->GetActorLocation(), SignalLoudness, Owner, SignalMaxRange, SignalTag);
}

void UEchoDistractionComponent::HandleDied()
{
	RemoveDistraction();
}

void UEchoDistractionComponent::RemoveDistraction()
{
	GetWorld()->GetTimerManager().ClearTimer(SignalTimerHandle);

	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}
