// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Character/PlayerCharacter.h"
#include "EchoPrototype/Character/EchoComponent.h"
#include "../Echo/EchoTypes.h"
#include "EchoCharacter.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;


UENUM(BlueprintType)
enum class EEchoVisualState : uint8
{
	Preview,
	Placed
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEchoPlaced);

UCLASS()
class ECHOPROTOTYPE_API AEchoCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AEchoCharacter();

	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE EEchoVisualState GetVisualState() const { return VisualState; }


	UFUNCTION(BlueprintCallable, Category = "Echo")
	void SetVisualState(EEchoVisualState NewState);


	UFUNCTION(BlueprintCallable, Category = "Echo")
	void SetPreviewValidity(bool bIsValid, EEchoType EchoType = EEchoType::Teleport);


	virtual void Jump() override {};

	UPROPERTY(BlueprintAssignable, Category = "Echo")
	FOnEchoPlaced OnPlaced;

protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PreviewOrbVisual;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	TObjectPtr<UMaterialInterface> PreviewMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	TObjectPtr<UMaterialInterface> TeleportPlacedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	TObjectPtr<UMaterialInterface> VisionPlacedMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PreviewMID;

	//Colors
	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	FLinearColor TeleportValidColor = FLinearColor(0.2f, 0.6f, 1.0f, 0.5f); // Blue

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	FLinearColor VisionValidColor = FLinearColor(0.2f, 1.0f, 0.3f, 0.5f);  // Green

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	FLinearColor InvalidPlacementColor = FLinearColor(1.0f, 0.2f, 0.2f, 0.5f); // Red


	UPROPERTY(EditDefaultsOnly, Category = "Echo|Vision")
	TObjectPtr<UMaterialInterface> EchoVisionPostProcessMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Movement")
	float EchoWalkSpeed = 250.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Echo|Movement")
	float MaxRangeFromOrigin = 800.0f;

	

public:
	virtual void Tick(float DeltaSeconds) override;

private:
	EEchoVisualState VisualState = EEchoVisualState::Preview;

	EEchoType ActiveEchoType = EEchoType::Teleport;

	FVector PlacedOriginLocation = FVector::ZeroVector;
};