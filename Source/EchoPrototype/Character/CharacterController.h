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
protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	UInputAction* CameraMoveAction;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void Move(const FInputActionValue& Value);
	void CameraMove(const FInputActionValue& Value);
	void Jump();

};
