// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractableInterface.h"
#include "Lever.generated.h"

class UStaticMeshComponent;
class ADoor;

UCLASS()
class ECHOPROTOTYPE_API ALever : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALever();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lever", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeverMesh;


	UPROPERTY(EditInstanceOnly, Category = "Lever")
	TObjectPtr<ADoor> LinkedDoor;


	UPROPERTY(EditAnywhere, Category = "Lever")
	float PulledPitchAngle = -45.0f;

private:
	bool bIsPulled = false;
	FRotator RestRotation = FRotator::ZeroRotator;

};
