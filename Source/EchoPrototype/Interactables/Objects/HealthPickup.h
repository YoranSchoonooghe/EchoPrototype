// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractableInterface.h"

#include "HealthPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOPROTOTYPE_API AHealthPickup : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPickup();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	virtual bool RequiresEchoVision_Implementation() override { return bRequiresEcho; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PickupMesh;


	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	float HealAmount = 50.0f;


	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	bool bFullHeal = false;


	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	bool bUsableByEcho = false;


	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	float RespawnDelay = 0.0f;


	UPROPERTY(EditAnywhere, Category = "Health Pickup")
	float SpinSpeed = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bRequiresEcho = false;

private:
	bool bIsAvailable = true;
	FTimerHandle RespawnTimerHandle;

	void SetAvailable(bool bAvailable);
	void Respawn();
};
