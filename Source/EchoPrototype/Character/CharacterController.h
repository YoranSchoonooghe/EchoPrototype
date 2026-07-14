// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerCharacter;

class UInteractionComponent;
class UInteractionPromptWidget;

UCLASS()
class ECHOPROTOTYPE_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterController();
protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;


	//Movement
	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> SneakAction;


	//Camera
	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* CameraMoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> ToggleCameraAction;

	//Echo
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> EchoAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookThroughEchoAction;

	//Combat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> StealthKillAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions")
	TObjectPtr<UInputAction> ClimbAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Interaction")
	TObjectPtr<UInputAction> InteractAction;


	UPROPERTY(EditDefaultsOnly, Category = "Interaction UI")
	TSubclassOf<UInteractionPromptWidget> InteractionPromptWidgetClass;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:

	//Movement
	void Move(const FInputActionValue& Value);
	void Jump();

	void StartSprinting();
	void StopSprinting();

	void StartSneaking();
	void StopSneaking();

	//Camera
	void CameraMove(const FInputActionValue& Value);
	void CameraCycle();

	//Echo
	void EchoPressed();
	void EchoReleased();
	void LookThroughEcho();

	//Combat
	void Attack();
	void AttackReleased();
	void StealthKill();
	void Climb();

	//Interact
	void Interact();

	UPROPERTY()
	TObjectPtr<APlayerCharacter> CachedPlayerCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UInteractionPromptWidget> InteractionPromptWidgetInstance;
};
