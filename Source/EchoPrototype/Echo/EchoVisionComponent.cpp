// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoVisionComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UEchoVisionComponent::UEchoVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APlayerController* UEchoVisionComponent::GetPlayerController() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetFirstPlayerController();
	}
	return nullptr;
}

void UEchoVisionComponent::ToggleEchoPossession(APawn* OriginalPlayerPawn)
{
	APawn* EchoPawn = Cast<APawn>(GetOwner());
	APlayerController* PC = GetPlayerController();

	if (!PC || !EchoPawn) return;

	if (bIsViewingThroughEcho)
	{
		ReturnViewToPlayer();
		return;
	}

	if (OriginalPlayerPawn)
	{
		CachedPlayerPawn = OriginalPlayerPawn;
	}

	PC->SetViewTargetWithBlend(EchoPawn, ViewBlendTime);
	PC->Possess(EchoPawn);

	bIsViewingThroughEcho = true;
}

void UEchoVisionComponent::ReturnViewToPlayer()
{
	APlayerController* PC = GetPlayerController();

	if (PC && CachedPlayerPawn)
	{
		PC->SetViewTargetWithBlend(CachedPlayerPawn.Get(), ViewBlendTime);
		PC->Possess(CachedPlayerPawn.Get());
	}

	bIsViewingThroughEcho = false;
}

void UEchoVisionComponent::AddEchoMoveInput(const FVector2D& Value)
{
	APawn* EchoPawn = Cast<APawn>(GetOwner());
	if (!EchoPawn) return;

	const FRotator Rotation = EchoPawn->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	EchoPawn->AddMovementInput(ForwardDirection, Value.Y);
	EchoPawn->AddMovementInput(RightDirection, Value.X);
}

void UEchoVisionComponent::AddEchoLookInput(float Rate, float Yaw)
{
	APawn* EchoPawn = Cast<APawn>(GetOwner());
	if (EchoPawn)
	{
		EchoPawn->AddControllerYawInput(Rate);
		EchoPawn->AddControllerPitchInput(Yaw);
	}
}