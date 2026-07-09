#include "CharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacterCameraManager.h"
#include "PlayerCharacter.h"


ACharacterController::ACharacterController()
{
	PlayerCameraManagerClass = APlayerCharacterCameraManager::StaticClass();
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterController::SetupInputComponent()
{
		Super::SetupInputComponent();

		if (IsLocalPlayerController())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			{
				for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			if (MoveAction)
				EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterController::Move);

			if (CameraMoveAction)
				EIC->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &ACharacterController::CameraMove);

			if (JumpAction)
				EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacterController::Jump);
		}
	
}

void ACharacterController::Move(const FInputActionValue& Value)
{
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn()))
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		if (MovementVector.Length() < 0.1f) return;

		PlayerChar->Move(MovementVector);
	}
}

void ACharacterController::CameraMove(const FInputActionValue & Value)
{
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn()))
	{
		FVector2D CameraMovementVector = Value.Get<FVector2D>();
		if (CameraMovementVector.Length() < 0.1f) return;

		PlayerChar->CameraMove(CameraMovementVector);
	}
}

void ACharacterController::Jump()
{
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerChar->Jump();
	}
}
