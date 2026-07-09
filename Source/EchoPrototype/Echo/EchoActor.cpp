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

	SetVisualState(EEchoVisualState::Preview);
	
}

void AEchoActor::SetVisualState(EEchoVisualState NewState)
{
	VisualState = NewState;

	switch (VisualState)
	{
	case EEchoVisualState::Preview:
		EchoVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EchoCamera->SetActive(false);

		EchoVisual->SetMaterial(0, PreviewMID ? PreviewMID : PreviewMaterial);
		break;

	case EEchoVisualState::Placed:
		EchoVisual->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		EchoCamera->SetActive(true);
		EchoVisual->SetMaterial(0, PlacedMaterial);
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