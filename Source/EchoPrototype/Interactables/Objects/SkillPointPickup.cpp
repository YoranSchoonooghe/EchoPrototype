// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillPointPickup.h"

#include "Components/StaticMeshComponent.h"
#include "../../Character/PlayerCharacter.h"
#include "../../SkillTree/SkillTreeComponent.h"
#include "../../SaveGame/SaveGameSubsystem.h"

ASkillPointPickup::ASkillPointPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
}

void ASkillPointPickup::BeginPlay()
{
	Super::BeginPlay();

	if (USaveGameSubsystem* SaveSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		if (SaveSubsystem->IsPickupCollected(GetFName()))
		{
			Destroy();
		}
	}
}

void ASkillPointPickup::Interact_Implementation(AActor* Interactor)
{
	if(APlayerCharacter* character = Cast<APlayerCharacter>(Interactor))
		character->GetSkillTreeComponent()->AddSkillPoints(SkillPointAmount);

	if (USaveGameSubsystem* SaveSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		SaveSubsystem->MarkPickupCollected(GetFName());
	}

	CollectPickup();
}


void ASkillPointPickup::CollectPickup()
{
	Destroy();
}
