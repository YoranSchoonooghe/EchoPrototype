// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoTeleportComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"


UEchoTeleportComponent::UEchoTeleportComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEchoTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsZooming)
	{
		UpdateTeleportFovEffect(DeltaTime);
	}
}

bool UEchoTeleportComponent::ExecuteTeleport(APawn* PlayerPawn)
{
	AActor* EchoActor = GetOwner();
	if (!PlayerPawn || !EchoActor)
	{
		return false;
	}

	const FVector TargetLocation = EchoActor->GetActorLocation();
	const FRotator TargetRotation = EchoActor->GetActorRotation();

	UCapsuleComponent* EchoCapsule = EchoActor->FindComponentByClass<UCapsuleComponent>();

	//Disable collision
	if (EchoCapsule)
	{
		EchoCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	StartTeleportFovEffect(PlayerPawn);

	if (!PlayerPawn->TeleportTo(TargetLocation, TargetRotation, false, false))
	{
		UE_LOG(LogTemp, Warning, TEXT("Teleport failed: Destination standard overlap check blocked movement."));

		if (EchoCapsule)
		{
			EchoCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		if (FovEffectCamera.IsValid())
		{
			FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);
		}
		bIsZooming = false;
		return false;
	}

	if (FovEffectCamera.IsValid())
	{
		FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);
		FovEffectCamera.Reset();
	}
	bIsZooming = false;

	return true;
}

void UEchoTeleportComponent::StartTeleportFovEffect(APawn* PlayerPawn)
{
	if (!PlayerPawn) return;

	UCameraComponent* Cam = PlayerPawn->FindComponentByClass<UCameraComponent>();
	if (!Cam) return;

	FovEffectCamera = Cam;
	FovEffectBaseFOV = Cam->FieldOfView;
	FovEffectStartFOV = TeleportSpikeFOV;
	Cam->SetFieldOfView(TeleportSpikeFOV);
	FovEffectElapsed = 0.0f;
	bIsZooming = true;
}

void UEchoTeleportComponent::UpdateTeleportFovEffect(float DeltaSeconds)
{
	if (!FovEffectCamera.IsValid())
	{
		bIsZooming = false;
		return;
	}

	FovEffectElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(FovEffectElapsed / TeleportZoomInDuration, 0.0f, 1.0f);
	const float Eased = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);
	FovEffectCamera->SetFieldOfView(FMath::Lerp(FovEffectStartFOV, FovEffectBaseFOV, Eased));

	if (Alpha >= 1.0f)
	{
		FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);
		bIsZooming = false;
		FovEffectCamera.Reset();
	}
}