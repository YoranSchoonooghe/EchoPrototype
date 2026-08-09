// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "SkillPointPickup.generated.h"

/**
 * 
 */
UCLASS()
class ECHOPROTOTYPE_API ASkillPointPickup : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	ASkillPointPickup();

	virtual void Interact_Implementation(AActor* Interactor) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	int32 SkillPointAmount = 1;

private:
	void CollectPickup();
};
