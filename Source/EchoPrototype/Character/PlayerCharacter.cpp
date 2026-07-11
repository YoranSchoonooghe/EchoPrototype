#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EchoComponent.h"

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

	// crouching
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// default speed
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.0f;
}

void APlayerCharacter::Move(const FVector2D& Value)
{
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

void APlayerCharacter::StartSprinting()
{
	if (!bIsCrouched)
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	}
}

void APlayerCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.0f; 
}

void APlayerCharacter::StartSneaking()
{
	Crouch();
}

void APlayerCharacter::StopSneaking()
{
	UnCrouch();
}

void APlayerCharacter::CameraMove(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(-Value.Y);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::EchoPressed()
{
	if (Echo)
		Echo->OnEchoPressed();
	
}

void APlayerCharacter::EchoReleased()
{
	if (Echo)
		Echo->OnEchoReleased();
}

void APlayerCharacter::LookThroughEcho()
{
	if (Echo)
		Echo->LookThroughEcho();
}

void APlayerCharacter::TeleportToEcho()
{
	if (Echo)
		Echo->TeleportToEcho();
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