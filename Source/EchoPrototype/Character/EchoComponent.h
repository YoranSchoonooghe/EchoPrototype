// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Echo/EchoCharacter.h"
#include "../Echo/EchoTypes.h"
#include "EchoComponent.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEchoPlacedSignature);

USTRUCT(BlueprintType)
struct FEchoAbilityUnlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo")
	bool bRequiresUnlock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo", meta = (EditCondition = "bRequiresUnlock"))
	FGameplayTag RequiredTag;
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
	void SetSelectedEchoType(EEchoType NewType);

	UFUNCTION(BlueprintPure, Category = "Echo")
	bool IsEchoTypeUnlocked(EEchoType Type) const;

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void TriggerPlacedEchoAbility();

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void AddEchoMoveInput(const FVector2D& Value);

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void AddEchoLookInput(float Rate, float Yaw);

	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE EEchoState GetEchoState() const { return EchoState; }

	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE bool IsViewingThroughEcho() const { return bIsViewingThroughEcho; }

	UFUNCTION(BlueprintPure, Category = "Echo")
	FORCEINLINE AEchoCharacter* GetActiveEcho() const { return ActiveEcho; }

	UFUNCTION(BlueprintCallable, Category = "Echo")
	void GetEchoViewPoint(FVector& OutLocation, FRotator& OutRotation) const;

	UPROPERTY(BlueprintAssignable, Category = "Echo")
	FOnEchoPlacedSignature OnPlaced;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTeleportComplete();

	UPROPERTY(EditDefaultsOnly, Category = "Echo")
	TSubclassOf<AEchoCharacter> EchoActorClass;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float EchoTraceDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float EchoTraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float GroundPlacementOffset = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Ground Detection")
	float MinWalkableNormalZ = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Echo|Ground Detection")
	float GroundSearchStep = 75.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Ground Detection")
	float GroundSearchHeight = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Ground Detection")
	float GroundSearchDepth = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Ground Detection")
	float MinPlacementDistanceFromPlayer = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float PreviewInterpSpeed = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float TapThreshold = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Echo")
	float ViewBlendTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportSpikeFOV = 130.0f;

	UPROPERTY(EditAnywhere, Category = "Echo|Teleport FX")
	float TeleportZoomInDuration = 0.3f;


	//types
	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	TSubclassOf<UActorComponent> TeleportComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	TSubclassOf<UActorComponent> VisionComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	TSubclassOf<UActorComponent> CombatComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	FEchoAbilityUnlock VisionUnlock;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	FEchoAbilityUnlock CombatUnlock;

	UPROPERTY(EditDefaultsOnly, Category = "Echo|Types")
	FEchoAbilityUnlock DistractionUnlock;


	//HUD
	UPROPERTY(EditDefaultsOnly, Category = "Echo|UI")
	TSubclassOf<class UUserWidget> SelectionMenuWidgetClass;

private:

	UPROPERTY(Transient)
	TObjectPtr<AEchoCharacter> ActiveEcho;

	EEchoState EchoState = EEchoState::Idle;
	double PressStartTime = 0.0;
	bool bIsViewingThroughEcho = false;
	bool bCurrentAimIsValid = false;


	//FOV Effects
	EEchoFOVEffect FovEffectPhase = EEchoFOVEffect::None;
	float FovEffectElapsed = 0.0f;
	float FovEffectStartFOV = 90.0f;
	float FovEffectBaseFOV = 90.0f;
	TWeakObjectPtr<UCameraComponent> FovEffectCamera;


	//Types
	EEchoType CurrentEchoType = EEchoType::Teleport;

	void AttachEchoAbility(AEchoCharacter* TargetEcho, EEchoType TypeToAttach);

	//HUD
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> SelectionMenuInstance;

	void ShowSelectionUI();
	void HideSelectionUI();

	void BeginAiming();
	void UpdateAimPreview(float DeltaSeconds);
	void PlaceEcho();
	void DestroyActiveEcho();
	void StartTeleportFovEffect();
	void UpdateTeleportFovEffect(float DeltaSeconds);

	bool TraceForEchoLocation(FVector& OutLocation, FRotator& OutRotation, bool& bOutValid) const;

	APawn* GetOwnerPawn() const;
	APlayerController* GetPlayerController() const;

	float GetEchoCapsuleHalfHeight() const;
};
