// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EchoComponentBase.h"
#include "EchoVisionComponent.generated.h"

class APawn;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEchoVisionAudioSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ECHOPROTOTYPE_API UEchoVisionComponent : public UEchoComponentBase
{
	GENERATED_BODY()

public:
	UEchoVisionComponent();

	UFUNCTION(BlueprintCallable, Category = "Echo|Vision")
	void ToggleEchoPossession(APawn* OriginalPlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Echo|Vision")
	void ReturnViewToPlayer();

	UFUNCTION(BlueprintCallable, Category = "Echo|Vision")
	void AddEchoMoveInput(const FVector2D& Value);

	UFUNCTION(BlueprintCallable, Category = "Echo|Vision")
	void AddEchoLookInput(float Rate, float Yaw);

	UFUNCTION(BlueprintPure, Category = "Echo|Vision")
	bool IsViewingThroughEcho() const { return bIsViewingThroughEcho; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Echo|Teleport|Audio")
	void PlayPossesEchoSound();

	UFUNCTION(BlueprintImplementableEvent, Category = "Echo|Teleport|Audio")
	void PlayReturnToPlayerSound();

	UPROPERTY(BlueprintAssignable, Category = "Echo|Vision|Audio")
	FEchoVisionAudioSignature OnVisionPossessionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Echo|Vision|Audio")
	FEchoVisionAudioSignature OnVisionPossessionEnded;

protected:
	UPROPERTY(EditAnywhere, Category = "Echo|Vision")
	float ViewBlendTime = 0.5f;

private:
	bool bIsViewingThroughEcho = false;

	UPROPERTY(Transient)
	TObjectPtr<APawn> CachedPlayerPawn;

	APlayerController* GetPlayerController() const;
};
