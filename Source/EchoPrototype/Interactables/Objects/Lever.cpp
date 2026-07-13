// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"
#include "Door.h"
#include "Components/StaticMeshComponent.h"

ALever::ALever()
{
	USceneComponent* NewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = NewRoot;

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	LeverMesh->SetupAttachment(RootComponent);

	bIsPulled = false;
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	if (LeverMesh)
	{
		RestRotation = LeverMesh->GetRelativeRotation();
	}
}

void ALever::Interact_Implementation(AActor* Interactor)
{
	bIsPulled = !bIsPulled;


	const FRotator NewRotation = RestRotation + FRotator(bIsPulled ? PulledPitchAngle : 0.0f, 0.0f, 0.0f);


	LeverMesh->SetRelativeRotation(NewRotation);

	if (LinkedDoor)
	{
		LinkedDoor->ToggleDoor();
	}
}

FText ALever::GetInteractionPrompt_Implementation() const
{
	return bIsPulled ? FText::FromString(TEXT("Press 'F' to Pull Lever")) : FText::FromString(TEXT("Press 'F' to Pull Lever"));
}