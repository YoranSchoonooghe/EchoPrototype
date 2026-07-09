#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 50.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	Echo = CreateDefaultSubobject<UEchoComponent>(TEXT("EchoComponent"));
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