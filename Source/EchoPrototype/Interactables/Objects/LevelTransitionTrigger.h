// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionTrigger.generated.h"

class UBoxComponent;

UCLASS()
class ECHOPROTOTYPE_API ALevelTransitionTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelTransitionTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Transition", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> TriggerZone;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	TSoftObjectPtr<UWorld> LevelToOpen;

	UFUNCTION()
	void OnTriggerZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	bool bHasTriggered = false;
};
