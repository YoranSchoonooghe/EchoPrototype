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

			//Movement
			if (MoveAction)
				EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterController::Move);

			if (JumpAction)
				EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacterController::Jump);
			
			if (SneakAction)
			{
				EIC->BindAction(SneakAction, ETriggerEvent::Started, this, &ACharacterController::StartSneaking);
				EIC->BindAction(SneakAction, ETriggerEvent::Completed, this, &ACharacterController::StopSneaking);
			}
			if (SprintAction)
			{
				EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ACharacterController::StartSprinting);
				EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACharacterController::StopSprinting);
			}

			//Camera
			if (CameraMoveAction)
				EIC->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &ACharacterController::CameraMove);

			if (ToggleCameraAction)
				EIC->BindAction(ToggleCameraAction, ETriggerEvent::Started, this, &ACharacterController::CameraCycle);

			//Echo
			if (EchoAction)
			{
 				EIC->BindAction(EchoAction, ETriggerEvent::Started, this, &ACharacterController::EchoPressed);
 				EIC->BindAction(EchoAction, ETriggerEvent::Completed, this, &ACharacterController::EchoReleased);
			}

			if (LookThroughEchoAction)
 				EIC->BindAction(LookThroughEchoAction, ETriggerEvent::Started, this, &ACharacterController::LookThroughEcho);

			//Combat
			if (AttackAction)
				EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ACharacterController::Attack);
		}
	
}

void ACharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CachedPlayerCharacter = Cast<APlayerCharacter>(InPawn);
}

void ACharacterController::OnUnPossess()
{
	Super::OnUnPossess();

	CachedPlayerCharacter = nullptr;
}

void ACharacterController::Move(const FInputActionValue& Value)
{
	if (!CachedPlayerCharacter) return;
	
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.Length() < 0.1f) return;

	CachedPlayerCharacter->Move(MovementVector);
}

void ACharacterController::CameraMove(const FInputActionValue & Value)
{
	if (!CachedPlayerCharacter) return;

	FVector2D CameraMovementVector = Value.Get<FVector2D>();
	if (CameraMovementVector.Length() < 0.1f) return;

	CachedPlayerCharacter->CameraMove(CameraMovementVector);
	
}

void ACharacterController::CameraCycle()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->CycleCameraPerspective();
}

void ACharacterController::Jump()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->Jump();
}

void ACharacterController::StartSprinting()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->StartSprinting();
}

void ACharacterController::StopSprinting()
{
	if (!CachedPlayerCharacter) return;
	
	CachedPlayerCharacter->StopSprinting();
	
}

void ACharacterController::StartSneaking()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->StartSneaking();
}

void ACharacterController::StopSneaking()
{
	if (!CachedPlayerCharacter) return;
	
	CachedPlayerCharacter->StopSneaking();
}

void ACharacterController::EchoPressed()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->EchoPressed();
}

void ACharacterController::EchoReleased()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->EchoReleased();
}

void ACharacterController::LookThroughEcho()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->LookThroughEcho();
}

void ACharacterController::Attack()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->AttackPressed();
}