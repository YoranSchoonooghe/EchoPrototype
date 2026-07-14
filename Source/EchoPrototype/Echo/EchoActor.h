//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
//#include "EchoActor.generated.h"
//
//class UCameraComponent;
//
//class UStaticMeshComponent;
//class USkeletalMeshComponent;
//class USkeletalMesh;
//class UMaterialInstanceDynamic;
//class UMaterialInterface;
//class UAIPerceptionStimuliSourceComponent;
//
//UENUM(BlueprintType)
//enum class EEchoVisualState : uint8
//{
//	Preview,	//Following player's aim
//	Placed
//};
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlacedSignature);
//
//UCLASS()
//class ECHOPROTOTYPE_API AEchoActor : public APawn
//{
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	AEchoActor();
//
//	UFUNCTION(BlueprintPure, Category = "Echo")
//	FORCEINLINE UCameraComponent* GetEchoCamera() const { return EchoCamera; }
//
//	UFUNCTION(BlueprintPure, Category = "Echo")
//	FORCEINLINE EEchoVisualState GetVisualState() const { return VisualState; }
//
//
//	UFUNCTION(BlueprintCallable, Category = "Echo")
//	void SetVisualState(EEchoVisualState NewState);
//
//
//	UFUNCTION(BlueprintCallable, Category = "Echo")
//	void SetPreviewValidity(bool bIsValid);
//
//	UFUNCTION(BlueprintCallable, Category = "Echo")
//	void AddCameraLookInput(float YawDelta, float PitchDelta);
//
//	UFUNCTION(BlueprintCallable, Category = "Echo")
//	void AddMoveInput(const FVector& WorldSpaceDelta);
//
//	UFUNCTION(BlueprintCallable, Category = "Echo")
//	void InitializeCameraFacing(const FRotator& WorldRotation);
//
//	UPROPERTY(BlueprintAssignable, Category = "Echo|VisualState")
//	FOnPlacedSignature OnPlaced;
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo", meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<USceneComponent> EchoRoot;
//
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo", meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<UStaticMeshComponent> EchoVisual;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo", meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<USkeletalMeshComponent> EchoSkeletalVisual;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo", meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<UCameraComponent> EchoCamera;
//
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	TObjectPtr<UStaticMesh> PreviewMesh;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	TObjectPtr<USkeletalMesh> PlacedMesh;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	TObjectPtr<UMaterialInterface> PreviewMaterial;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	TObjectPtr<UMaterialInterface> PlacedMaterial;
//
//	UPROPERTY(Transient)
//	TObjectPtr<UMaterialInstanceDynamic> PreviewMID;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	FLinearColor ValidPlacementColor = FLinearColor(0.2f, 0.6f, 1.0f, 0.5f);
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Visuals")
//	FLinearColor InvalidPlacementColor = FLinearColor(1.0f, 0.2f, 0.2f, 0.5f);
//
//
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Camera")
//	float MinLookPitch = -80.0f;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Camera")
//	float MaxLookPitch = 80.0f;
//
//
//
//	UPROPERTY(EditDefaultsOnly, Category = "Echo|Movement")
//	float MaxDriftDistance = 150.0f;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Echo|Perception")
//	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
//
//private:
//	EEchoVisualState VisualState = EEchoVisualState::Preview;
//
//	FRotator CameraLookRotation = FRotator::ZeroRotator;
//
//	FVector PlacedOriginLocation = FVector::ZeroVector;
//};
