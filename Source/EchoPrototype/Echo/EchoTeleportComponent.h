// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/PostProcessComponent.h"

#include "NiagaraSystem.h"

#include "EchoTeleportComponent.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportComplete);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ECHOPROTOTYPE_API UEchoTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEchoTeleportComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	bool ExecuteTeleport(APawn* PlayerPawn);

	UPROPERTY(BlueprintAssignable, Category = "Teleport")
	FOnTeleportComplete OnTeleportComplete;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	float TeleportSpikeFOV = 115.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	float PreTeleportDelay = 0.06f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	float TeleportRecoveryDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	TSubclassOf<UCameraShakeBase> ArrivalCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	float MaxChromaticAberration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Visuals")
	float MaxMotionBlur = 1.0f;


	//VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Juice")
	UNiagaraSystem* DepartureFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport|Juice")
	UNiagaraSystem* ArrivalFX;

private:
	void PerformActualTeleport();
	void StartTeleportEffects(APawn* PlayerPawn);
	void UpdateTeleportEffects(float DeltaSeconds);
	void ResetPostProcessing();

	void SetPlayerVisible(APawn* PlayerPawn, bool bVisible);

	TWeakObjectPtr<APawn> CachedPlayerPawn;
	TWeakObjectPtr<UCameraComponent> FovEffectCamera;
	TWeakObjectPtr<UPostProcessComponent> DynamicPostProcessComp;

	float FovEffectBaseFOV = 90.0f;
	float FovEffectStartFOV = 115.0f;
	float EffectElapsed = 0.0f;
	bool bIsRecovering = false;

	FTimerHandle TeleportTimerHandle;
};
