#include "CharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacterCameraManager.h"
#include "PlayerCharacter.h"

#include "../Echo/EchoCharacter.h"
#include "EchoComponent.h"

#include "../HUD/InteractionPromptWidget.h"
#include "../HUD/MenuFlowSubsystem.h"
#include "../HUD/States/MenuStateBase.h"
#include "../Interactables/InteractionComponent.h"
#include "../SaveGame/SaveGameSubsystem.h"
#include "Blueprint/UserWidget.h"



ACharacterController::ACharacterController()
{
	PlayerCameraManagerClass = APlayerCharacterCameraManager::StaticClass();
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultRootMenuState)
	{
		if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
		{
			Flow->SetRootState(DefaultRootMenuState);
		}
	}
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
				EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacterController::Jump);
			
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

			if (SelectTeleportEchoAction)
				EIC->BindAction(SelectTeleportEchoAction, ETriggerEvent::Started, this, &ACharacterController::SelectTeleportEcho);

			if (SelectVisionEchoAction)
				EIC->BindAction(SelectVisionEchoAction, ETriggerEvent::Started, this, &ACharacterController::SelectVisionEcho);
			
			if (SelectCombatEchoAction)
				EIC->BindAction(SelectCombatEchoAction, ETriggerEvent::Started, this, &ACharacterController::SelectCombatEcho);

			if (SelectDistractionEchoAction)
				EIC->BindAction(SelectDistractionEchoAction, ETriggerEvent::Started, this, &ACharacterController::SelectDistractionEcho);

			if (UseEchoAbilityAction)
				EIC->BindAction(UseEchoAbilityAction, ETriggerEvent::Started, this, &ACharacterController::UseEchoAbility);

			//Combat
			if (AttackAction)
			{
				EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ACharacterController::Attack);
				EIC->BindAction(AttackAction, ETriggerEvent::Completed, this, &ACharacterController::AttackReleased);
			}

			if (StealthKillAction)
				EIC->BindAction(StealthKillAction, ETriggerEvent::Started, this, &ACharacterController::StealthKill);

			if (ClimbAction)
				EIC->BindAction(ClimbAction, ETriggerEvent::Started, this, &ACharacterController::Climb);

			//Interact
			if (InteractAction)
				EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ACharacterController::Interact);

			//Menu
			if (PauseAction)
				EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &ACharacterController::RequestPause);
		}
	
}

void ACharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CachedPlayerCharacter = Cast<APlayerCharacter>(InPawn);


	//Handle standard logic
	if (InPawn && !InPawn->IsA<AEchoCharacter>())
	{
		if (APlayerCharacter* RealPlayer = Cast<APlayerCharacter>(InPawn))
		{
			CachedEchoComponent = RealPlayer->FindComponentByClass<UEchoComponent>();
		}

		if (USaveGameSubsystem* SaveGameSys = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
		{
			SaveGameSys->ApplyPendingLoadIfAny(CachedPlayerCharacter);
		}

		if (AutosaveInterval > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(AutosaveTimerHandle, this, &ACharacterController::PerformAutosave, AutosaveInterval, true);
		}
	}

	//UI Widget
	if (InteractionPromptWidgetClass && !InteractionPromptWidgetInstance)
	{
		InteractionPromptWidgetInstance = CreateWidget<UInteractionPromptWidget>(this, InteractionPromptWidgetClass);
		if (InteractionPromptWidgetInstance)
		{
			InteractionPromptWidgetInstance->AddToViewport();
		}
	}

	//Toast UI
	if (ToastWidgetClass && !ToastWidgetInstance)
	{
		ToastWidgetInstance = CreateWidget<UUserWidget>(this, ToastWidgetClass);
		if (ToastWidgetInstance)
		{
			ToastWidgetInstance->AddToViewport();
		}
	}

	//Interaction
	if (InteractionPromptWidgetInstance)
	{
		UInteractionComponent* Interaction = InPawn->FindComponentByClass<UInteractionComponent>();
		if (!Interaction)
		{
			TArray<UInteractionComponent*> InteractionComps;
			InPawn->GetComponents<UInteractionComponent>(InteractionComps, true);
			if (InteractionComps.Num() > 0)
			{
				Interaction = InteractionComps[0];
			}
		}

		if (Interaction)
		{
			InteractionPromptWidgetInstance->InitializeForInteractionComponent(Interaction);
		}
	}
}

void ACharacterController::OnUnPossess()
{
	Super::OnUnPossess();

	GetWorldTimerManager().ClearTimer(AutosaveTimerHandle);

	CachedPlayerCharacter = nullptr;
}

void ACharacterController::PerformAutosave()
{
	if (!CachedPlayerCharacter)
	{
		return;
	}

	if (USaveGameSubsystem* SaveGameSys = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameSubsystem>() : nullptr)
	{
		SaveGameSys->SaveGame(CachedPlayerCharacter, AutosaveSlotName);
	}
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
	if (CachedEchoComponent)
	{
		CachedEchoComponent->OnEchoPressed();
	}
}

void ACharacterController::EchoReleased()
{
	if (CachedEchoComponent)
	{
		CachedEchoComponent->OnEchoReleased();
	}
}

void ACharacterController::SelectTeleportEcho()
{
	if (CachedEchoComponent && CachedEchoComponent->GetEchoState() == EEchoState::Aiming)
	{
		CachedEchoComponent->SetSelectedEchoType(EEchoType::Teleport);
	}
}

void ACharacterController::SelectVisionEcho()
{
	if (CachedEchoComponent && CachedEchoComponent->GetEchoState() == EEchoState::Aiming)
	{
		CachedEchoComponent->SetSelectedEchoType(EEchoType::Vision);
	}
}

void ACharacterController::SelectCombatEcho()
{
	if (CachedEchoComponent && CachedEchoComponent->GetEchoState() == EEchoState::Aiming)
	{
		CachedEchoComponent->SetSelectedEchoType(EEchoType::Combat);
	}
}

void ACharacterController::SelectDistractionEcho()
{
	if (CachedEchoComponent && CachedEchoComponent->GetEchoState() == EEchoState::Aiming)
	{
		CachedEchoComponent->SetSelectedEchoType(EEchoType::Distraction);
	}
}

void ACharacterController::UseEchoAbility()
{
	if (CachedEchoComponent)
	{
		CachedEchoComponent->TriggerPlacedEchoAbility();
	}
}

void ACharacterController::Attack()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->AttackPressed();
}

void ACharacterController::AttackReleased()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->AttackReleased();
}

void ACharacterController::StealthKill()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->StealthKillPressed();
}

void ACharacterController::Climb()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->ClimbPressed();
}

void ACharacterController::Interact()
{
	if (UInteractionComponent* Interaction = GetPawn() ? GetPawn()->FindComponentByClass<UInteractionComponent>() : nullptr)
	{
		Interaction->OnInteractPressed();
	}
}

void ACharacterController::RequestPause()
{
	if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
	{
		Flow->RequestBack();
	}
}