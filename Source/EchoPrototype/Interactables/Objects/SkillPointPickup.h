// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupActor.h"
#include "SkillPointPickup.generated.h"

/**
 *
 */
UCLASS()
class ECHOPROTOTYPE_API ASkillPointPickup : public APickupActor
{
	GENERATED_BODY()

public:
	ASkillPointPickup();

protected:
	virtual void OnPickedUp(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	int32 SkillPointAmount = 1;
};
