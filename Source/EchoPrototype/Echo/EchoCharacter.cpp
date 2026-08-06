// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "EchoPrototype/Character/PlayerCharacter.h"
#include "EngineUtils.h"
#include "EchoVisionComponent.h"

AEchoCharacter::AEchoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PreviewOrbVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewOrbVisual"));
	PreviewOrbVisual->SetupAttachment(GetRootComponent());
	PreviewOrbVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewOrbVisual->SetCollisionResponseToAllChannels(ECR_Ignore);


	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = 0.0f;

		CameraBoom->SetupAttachment(GetMesh(), TEXT("head"));

		CameraBoom->SocketOffset = FVector(10.f, 0.f, 0.f);
	}

	if (FollowCamera)
	{
		FollowCamera->bUsePawnControlRotation = true;
	}

	bUseControllerRotationYaw = true;
}

void AEchoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = EchoWalkSpeed;
	}

	if (PreviewMaterial && PreviewOrbVisual)
	{
		PreviewMID = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
		PreviewOrbVisual->SetMaterial(0, PreviewMID ? PreviewMID : PreviewMaterial);
	}

	if (PreviewMesh && PreviewOrbVisual)
	{
		PreviewOrbVisual->SetStaticMesh(PreviewMesh);
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

	USkeletalMeshComponent* BodyMesh = GetMesh();
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	switch (VisualState)
	{
	case EEchoVisualState::Preview:

		if (PreviewOrbVisual)
		{
			PreviewOrbVisual->SetVisibility(true);
		}

		if (BodyMesh)
		{
			BodyMesh->SetVisibility(false);
			BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		
		if (PreviewOrbVisual)
		{
			PreviewOrbVisual->SetVisibility(false);
		}

		if (BodyMesh)
		{
			BodyMesh->SetVisibility(true);
			BodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			UMaterialInterface* MaterialToApply = nullptr;

			if (FindComponentByClass<UEchoVisionComponent>() || ActiveEchoType == EEchoType::Vision)
			{
				MaterialToApply = VisionPlacedMaterial;
			}
			else
			{
				MaterialToApply = TeleportPlacedMaterial;
			}

			if (MaterialToApply)
			{
				BodyMesh->SetOverlayMaterial(nullptr);
				const int32 NumMaterials = BodyMesh->GetNumMaterials();
				for (int32 i = 0; i < NumMaterials; ++i)
				{
					BodyMesh->SetMaterial(i, MaterialToApply);
				}
			}
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

void AEchoCharacter::SetPreviewValidity(bool bIsValid, EEchoType EchoType)
{
	ActiveEchoType = EchoType;

	if (PreviewMID)
	{
		FLinearColor SelectedColor;

		if (!bIsValid)
		{
			SelectedColor = InvalidPlacementColor;
		}
		else
		{
			SelectedColor = (ActiveEchoType == EEchoType::Vision) ? VisionValidColor : TeleportValidColor;
		}

		PreviewMID->SetVectorParameterValue(TEXT("TintColor"), SelectedColor);
	}
}