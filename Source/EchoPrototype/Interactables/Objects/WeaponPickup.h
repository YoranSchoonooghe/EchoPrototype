// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupActor.h"
#include "WeaponPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOPROTOTYPE_API AWeaponPickup : public APickupActor
{
	GENERATED_BODY()

public:
	AWeaponPickup();

protected:
	virtual void OnPickedUp(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;
};
