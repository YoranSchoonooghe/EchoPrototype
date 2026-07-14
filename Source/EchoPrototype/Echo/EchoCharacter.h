// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Character/PlayerCharacter.h"
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
	void SetPreviewValidity(bool bIsValid);


	virtual void Jump() override {}

	virtual void SwapPressed() override;

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
	TObjectPtr<UMaterialInterface> PlacedMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PreviewMID;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	FLinearColor ValidPlacementColor = FLinearColor(0.2f, 0.6f, 1.0f, 0.5f);

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
	FLinearColor InvalidPlacementColor = FLinearColor(1.0f, 0.2f, 0.2f, 0.5f);


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

	FVector PlacedOriginLocation = FVector::ZeroVector;
};