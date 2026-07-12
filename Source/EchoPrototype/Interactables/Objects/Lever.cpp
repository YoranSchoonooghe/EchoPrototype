// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"
#include "Door.h"
#include "Components/StaticMeshComponent.h"

ALever::ALever()
{
	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	SetRootComponent(LeverMesh);
}

void ALever::Interact_Implementation(AActor* Interactor)
{
	bIsPulled = !bIsPulled;


	if (RestRotation.IsZero() && !LeverMesh->GetRelativeRotation().IsZero())
	{
		RestRotation = LeverMesh->GetRelativeRotation();
	}
	const FRotator NewRotation = RestRotation + FRotator(bIsPulled ? PulledPitchAngle : 0.0f, 0.0f, 0.0f);
	LeverMesh->SetRelativeRotation(NewRotation);

	if (LinkedDoor)
	{
		LinkedDoor->ToggleDoor();
	}
}

FText ALever::GetInteractionPrompt_Implementation() const
{
	return bIsPulled ? FText::FromString(TEXT("Push Lever")) : FText::FromString(TEXT("Pull Lever"));
}