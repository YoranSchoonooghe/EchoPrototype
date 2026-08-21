// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponPickup.h"

#include "Components/StaticMeshComponent.h"
#include "../../Character/PlayerCharacter.h"
#include "../../Combat/WeaponData.h"

AWeaponPickup::AWeaponPickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
}

void AWeaponPickup::OnPickedUp(AActor* Interactor)
{
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(Interactor))
	{
		Character->EquipWeapon(WeaponData);
	}
}
