// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoVisionComponent.generated.h"

class APawn;
class APlayerController;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ECHOPROTOTYPE_API UEchoVisionComponent : public UActorComponent
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

protected:
	UPROPERTY(EditAnywhere, Category = "Echo|Vision")
	float ViewBlendTime = 0.5f;

private:
	bool bIsViewingThroughEcho = false;

	UPROPERTY(Transient)
	TObjectPtr<APawn> CachedPlayerPawn;

	APlayerController* GetPlayerController() const;
};
