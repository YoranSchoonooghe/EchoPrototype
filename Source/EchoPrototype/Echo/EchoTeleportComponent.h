// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoTeleportComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API UEchoTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEchoTeleportComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Echo|Teleport")
	bool ExecuteTeleport(APawn* PlayerPawn);

protected:
	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportSpikeFOV = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportZoomInDuration = 0.3f;

private:
	void StartTeleportFovEffect(APawn* PlayerPawn);
	void UpdateTeleportFovEffect(float DeltaSeconds);

	bool bIsZooming = false;
	float FovEffectElapsed = 0.0f;
	float FovEffectStartFOV = 90.0f;
	float FovEffectBaseFOV = 90.0f;
	TWeakObjectPtr<UCameraComponent> FovEffectCamera;
};
