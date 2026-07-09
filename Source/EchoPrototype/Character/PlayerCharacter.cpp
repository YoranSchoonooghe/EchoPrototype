
#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerCharacter::Move(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		const FVector ForwardDirection = FVector(1.0f, 0.0f, 0.0f);
		const FVector RightDirection = FVector(0.0f, 1.0f, 0.0f);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void APlayerCharacter::CameraMove(const FVector2D& Value)
{
	// TODO
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

