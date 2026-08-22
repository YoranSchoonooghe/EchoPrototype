// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveZone.h"
#include "Components/BoxComponent.h"
#include "SaveGameSubsystem.h"
#include "../Character/PlayerCharacter.h"

ASaveZone::ASaveZone()
{
	SaveVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SaveVolume"));
	SetRootComponent(SaveVolume);
	SaveVolume->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
	SaveVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SaveVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	SaveVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SaveVolume->SetGenerateOverlapEvents(true);
}

void ASaveZone::BeginPlay()
{
	Super::BeginPlay();

	SaveVolume->OnComponentBeginOverlap.AddDynamic(this, &ASaveZone::OnSaveVolumeBeginOverlap);
}

void ASaveZone::OnSaveVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (USaveGameSubsystem* SaveGameSys = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		SaveGameSys->SaveGame(Player, SaveSlotName);
		OnGameSaved();
	}
}
