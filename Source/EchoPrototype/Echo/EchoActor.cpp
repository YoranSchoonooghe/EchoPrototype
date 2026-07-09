// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoActor.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AEchoActor::AEchoActor()
{
	PrimaryActorTick.bCanEverTick = false;

	EchoRoot = CreateDefaultSubobject<USceneComponent>(TEXT("EchoRoot"));
	SetRootComponent(EchoRoot);


	EchoVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbVisual"));
	EchoVisual->SetupAttachment(EchoRoot);
	EchoVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EchoVisual->SetCollisionResponseToAllChannels(ECR_Ignore);

	EchoSkeletalVisual = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CreatureSkeletalVisual"));
	EchoSkeletalVisual->SetupAttachment(EchoRoot);
	EchoSkeletalVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EchoSkeletalVisual->SetCollisionResponseToAllChannels(ECR_Ignore);
	EchoSkeletalVisual->SetVisibility(false);


	EchoCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("EchoCamera"));
	EchoCamera->SetupAttachment(EchoRoot);
	EchoCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	EchoCamera->bUsePawnControlRotation = false;
	EchoCamera->SetFieldOfView(90.0f);

	EchoCamera->SetActive(false);
}

// Called when the game starts or when spawned
void AEchoActor::BeginPlay()
{
	Super::BeginPlay();

	if (PreviewMaterial && EchoVisual)
	{
		PreviewMID = UMaterialInstanceDynamic::Create(PreviewMaterial, this);
	}

	if (PlacedMesh && EchoSkeletalVisual)
	{
		EchoSkeletalVisual->SetSkeletalMesh(PlacedMesh);
	}

	SetVisualState(EEchoVisualState::Preview);
	
}

void AEchoActor::SetVisualState(EEchoVisualState NewState)
{
	VisualState = NewState;

	switch (VisualState)
	{
	case EEchoVisualState::Preview:
		EchoVisual->SetVisibility(true);
		EchoVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (PreviewMesh)
		{
			EchoVisual->SetStaticMesh(PreviewMesh);
		}
		EchoVisual->SetMaterial(0, PreviewMID ? PreviewMID : PreviewMaterial);


		EchoSkeletalVisual->SetVisibility(false);
		EchoSkeletalVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		EchoCamera->SetActive(false);
		break;

	case EEchoVisualState::Placed:
		EchoVisual->SetVisibility(false);
		EchoVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		EchoSkeletalVisual->SetVisibility(true);
		EchoSkeletalVisual->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (PlacedMaterial)
		{

			EchoSkeletalVisual->SetOverlayMaterial(nullptr);
			const int NumMaterials = EchoSkeletalVisual->GetNumMaterials();

			for (int i = 0; i < NumMaterials; ++i)
			{
				EchoSkeletalVisual->SetMaterial(i, PlacedMaterial);
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WARNING: PlacedMaterial is not assigned in the Blueprint defaults!"));
			}
		}

		EchoCamera->SetActive(true);
		break;
	}
}

void AEchoActor::SetPreviewValidity(bool bIsValid)
{
	if (PreviewMID)
	{
		PreviewMID->SetVectorParameterValue(TEXT("TintColor"), bIsValid ? ValidPlacementColor : InvalidPlacementColor);
	}
}