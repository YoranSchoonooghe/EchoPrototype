// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CapsuleComponent.h"

AEchoCharacter::AEchoCharacter()
{
}

void AEchoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = EchoWalkSpeed;
	}

	if (PreviewMaterial)
	{
		PreviewMID = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
	}

	if (EchoVisionPostProcessMaterial && FollowCamera)
	{
		FollowCamera->PostProcessSettings.AddBlendable(EchoVisionPostProcessMaterial, 1.0f);
	}

	SetVisualState(EEchoVisualState::Preview);
}

void AEchoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (VisualState != EEchoVisualState::Placed)
	{
		return;
	}

	const FVector CurrentLocation = GetActorLocation();
	const FVector OffsetFromOrigin = CurrentLocation - PlacedOriginLocation;

	if (OffsetFromOrigin.SizeSquared() > FMath::Square(MaxRangeFromOrigin))
	{
		const FVector ClampedLocation = PlacedOriginLocation + OffsetFromOrigin.GetClampedToMaxSize(MaxRangeFromOrigin);
		SetActorLocation(ClampedLocation, false);
	}
}

void AEchoCharacter::SetVisualState(EEchoVisualState NewState)
{
	VisualState = NewState;

	USkeletalMeshComponent* MeshComp = GetMesh();
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	switch (VisualState)
	{
	case EEchoVisualState::Preview:
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (MeshComp)
		{
			MeshComp->SetMaterial(0, PreviewMID ? PreviewMID : PreviewMaterial);
		}
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->DisableMovement();
		}
		break;

	case EEchoVisualState::Placed:
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		if (MeshComp)
		{
			MeshComp->SetMaterial(0, PlacedMaterial);
		}
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->SetMovementMode(MOVE_Walking);
		}

		PlacedOriginLocation = GetActorLocation();

		OnPlaced.Broadcast();

		break;
	}
}

void AEchoCharacter::SetPreviewValidity(bool bIsValid)
{
	if (PreviewMID)
	{
		PreviewMID->SetVectorParameterValue(TEXT("TintColor"), bIsValid ? ValidPlacementColor : InvalidPlacementColor);
	}
}