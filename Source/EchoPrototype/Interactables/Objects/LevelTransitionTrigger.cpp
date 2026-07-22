// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelTransitionTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EchoPrototype/Character/PlayerCharacter.h"

ALevelTransitionTrigger::ALevelTransitionTrigger()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(Root);

	TriggerZone->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ALevelTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionTrigger::OnTriggerZoneBeginOverlap);
}

void ALevelTransitionTrigger::OnTriggerZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered || LevelToOpen.IsNull() || !Cast<APlayerCharacter>(OtherActor))
	{
		return;
	}

	bHasTriggered = true;
	UGameplayStatics::OpenLevel(this, FName(*LevelToOpen.ToSoftObjectPath().GetLongPackageName()));
}
