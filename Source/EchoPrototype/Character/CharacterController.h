// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class UInputMappingContext;
class UInputAction;

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
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* JumpAction;


	//Camera
	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* CameraMoveAction;


	//Echo
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> EchoAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookThroughEchoAction;


	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	//Movement
	void Move(const FInputActionValue& Value);
	void Jump();

	//Camera
	void CameraMove(const FInputActionValue& Value);

	//Echo
	void EchoPressed();
	void EchoReleased();
	void LookThroughEcho();
};
