// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoComponent.generated.h"

class AEchoActor;
class UCameraComponent;


enum class EEchoVisualState : uint8;

UENUM(BlueprintType)
enum class EEchoState : uint8
{
	Idle,
	Aiming,
	Placed
};

UENUM(BlueprintType)
enum class EEchoFOVEffect : uint8
{
	None,
	ZoomingIn
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API UEchoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEchoComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Echo")
	void OnEchoPressed();

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void OnEchoReleased();

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void LookThroughEcho();

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void TeleportToEcho();

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void ReturnViewToSelf();


	UFUNCTION(BlueprintCallable, Category = "Echo")
	void AddEchoLookInput(float yawDelta, float PitchDelta);


	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE EEchoState GetEchoState() const { return EchoState; }

	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE bool IsViewingThroughEcho() const { return bIsViewingThroughEcho; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Echo")
	TSubclassOf<AEchoActor> EchoActorClass;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float EchoTraceDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float EchoTraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float PreviewInterpSpeed = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float TapThreshold = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float ViewBlendTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportFOV = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportZoomDuration = 0.3f;

private:
	UPROPERTY(Transient)
	TObjectPtr<AEchoActor> ActiveEcho;

	EEchoState EchoState = EEchoState::Idle;
	double PressStartTime = 0.0;
	bool bIsViewingThroughEcho = false;


	EEchoFOVEffect FovEffect = EEchoFOVEffect::None;
	float FovEffectElapsed = 0.0f;
	float FovEffectStartFOV = 90.0f;
	float FovEffectBaseFOV = 90.0f;
	UCameraComponent* Camera;

	void BeginAiming();
	void UpdateAimPreview(float DeltaSeconds);
	void PlaceEcho();
	void DestroyActiveEcho();

	void StartTeleportFovEffect();
	void UpdateTeleportFovEffect(float DeltaTime);

	bool TraceForEchoLocation(FVector& OutLocation, FRotator& OutRotation, bool& bOutValid) const;


	APawn* GetOwnerPawn() const;
};
