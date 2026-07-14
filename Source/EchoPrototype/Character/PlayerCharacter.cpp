#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EchoComponent.h"
#include "../Combat/CombatComponent.h"
#include "../Combat/HealthComponent.h"
#include "../Combat/StealthKillComponent.h"
#include "../Movement/ClimbingComponent.h"
#include "States/PlayerStateBase.h"
#include "States/PlayerStates.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true; 
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	// over the shoulder
	CurrentPerspective = ECameraPerspective::ThirdPerson;
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 50.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	Echo = CreateDefaultSubobject<UEchoComponent>(TEXT("EchoComponent"));
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StealthKill = CreateDefaultSubobject<UStealthKillComponent>(TEXT("StealthKillComponent"));
	Climbing = CreateDefaultSubobject<UClimbingComponent>(TEXT("ClimbingComponent"));
	Interaction = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComponent"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();

	// crouching
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// default speed
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.0f;
}

void APlayerCharacter::Move(const FVector2D& Value)
{
	if (Climbing && Climbing->IsHanging())
	{
		Climbing->HandleShimmyInput(Value);
		return;
	}

	if (CurrentState && !CurrentState->CanMove()) return;


	if (Echo && Echo->IsViewingThroughEcho())
	{
		Echo->AddEchoMoveInput(Value);
		return;
	}


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void APlayerCharacter::Jump()
{
	if (Climbing && Climbing->IsHanging())
	{
		Climbing->JumpFromLedge();
		return;
	}

	Super::Jump();
}

void APlayerCharacter::StartSprinting()
{
	if (CurrentState) ChangeState(CurrentState->OnSprintPressed(this));
}

void APlayerCharacter::StopSprinting()
{
	if (CurrentState) ChangeState(CurrentState->OnSprintReleased(this));
}

void APlayerCharacter::StartSneaking()
{
	if (CurrentState) ChangeState(CurrentState->OnSneakPressed(this));
}

void APlayerCharacter::StopSneaking()
{
	if (CurrentState) ChangeState(CurrentState->OnSneakReleased(this));
}

void APlayerCharacter::CameraMove(const FVector2D& Value)
{
	if (Echo && Echo->IsViewingThroughEcho())
	{
		Echo->AddEchoLookInput(Value.X, -Value.Y);
		return;
	}

	if (Controller != nullptr)
	{
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(-Value.Y);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ChangeState(NewObject<UPlayerState_IdleWalk>(this));

	if (Health)
	{
		Health->OnDeath.AddDynamic(this, &APlayerCharacter::HandleDeath);
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.0f,
		this,
		0.0f,
		TEXT("Landed")
	);
}

void APlayerCharacter::HandleDeath()
{
	// Dying while hanging would otherwise leave the corpse floating in flying mode.
	if (Climbing && Climbing->IsHanging())
	{
		Climbing->CancelHanging();
	}

	ChangeState(NewObject<UPlayerState_Dead>(this));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		ChangeState(CurrentState->UpdateState(this, DeltaTime));
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::ChangeState(UPlayerStateBase* NewState)
{
	if (NewState == nullptr || NewState == CurrentState) return;

	if (CurrentState)
	{
		CurrentState->ExitState(this);
	}

	CurrentState = NewState;

	if (CurrentState)
	{
		CurrentState->EnterState(this);
	}
}

void APlayerCharacter::EchoPressed()
{
	if (!Echo) return;

	if (Climbing && Climbing->IsHanging() && Echo->GetEchoState() == EEchoState::Placed)
	{
		Climbing->CancelHanging();
	}

	Echo->OnEchoPressed();
}

void APlayerCharacter::EchoReleased()
{
	if (Echo)
		Echo->OnEchoReleased();
}

void APlayerCharacter::SwapPressed()
{
	if (Echo)
		Echo->SwapPressed();
}

void APlayerCharacter::TeleportToEcho()
{
	if (!Echo) return;

	if (Climbing && Climbing->IsHanging() && Echo->GetEchoState() == EEchoState::Placed)
	{
		Climbing->CancelHanging();
	}

	Echo->TeleportToEcho();
}

void APlayerCharacter::AttackPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnAttackPressed(this));
}

void APlayerCharacter::AttackReleased()
{
	if (CurrentState) ChangeState(CurrentState->OnAttackReleased(this));
}

void APlayerCharacter::StealthKillPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnStealthKillPressed(this));
}

void APlayerCharacter::ClimbPressed()
{
	if (CurrentState) ChangeState(CurrentState->OnClimbPressed(this));
}

bool APlayerCharacter::IsAttacking() const
{
	return Combat && Combat->IsAttacking();
}

void APlayerCharacter::CycleCameraPerspective()
{
	switch (CurrentPerspective)
	{
	case ECameraPerspective::ThirdPerson:
		// SWITCHING TO TOP-DOWN
		CameraBoom->TargetArmLength = 800.0f; 
		CameraBoom->SocketOffset = FVector::ZeroVector; 
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

		CurrentPerspective = ECameraPerspective::TopDown;
		break;

	case ECameraPerspective::TopDown:
		// SWITCHING TO FIRST-PERSON
		CameraBoom->TargetArmLength = -10.0f;
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, BaseEyeHeight);
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);

		GetMesh()->SetOwnerNoSee(true);

		CurrentPerspective = ECameraPerspective::FirstPerson;
		break;

	case ECameraPerspective::FirstPerson:
		// SWITCHING BACK TO THIRD-PERSON
		CameraBoom->TargetArmLength = 250.0f; 
		CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 50.0f);
		CameraBoom->bUsePawnControlRotation = true;

		GetMesh()->SetOwnerNoSee(false);

		CurrentPerspective = ECameraPerspective::ThirdPerson;
		break;
	}
}