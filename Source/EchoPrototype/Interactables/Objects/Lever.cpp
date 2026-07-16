// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"
#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "EchoPrototype/Echo/EchoCharacter.h"
#include "EchoPrototype/Character/EchoComponent.h"

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
	if (!Interactor) return;

	bool bCanInteract = false;

	if (!bRequiresEcho)
	{
		bCanInteract = true;
	}
	else
	{
		bool bIsEcho = Interactor->IsA<AEchoCharacter>();
		bool bIsPlayerViewingThroughEcho = false;

		if (UEchoComponent* EchoComp = Interactor->FindComponentByClass<UEchoComponent>())
		{
			if (EchoComp->IsViewingThroughEcho())
			{
				bIsPlayerViewingThroughEcho = true;
			}
		}

		if (bIsEcho || bIsPlayerViewingThroughEcho)
		{
			bCanInteract = true;
		}
	}

	if (bCanInteract)
	{
		bIsPulled = !bIsPulled;

		const FRotator NewRotation = RestRotation + FRotator(bIsPulled ? PulledPitchAngle : 0.0f, 0.0f, 0.0f);
		LeverMesh->SetRelativeRotation(NewRotation);

		if (LinkedDoor)
		{
			LinkedDoor->ToggleDoor();
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Lever requires Echo"));
		}
	}
}

FText ALever::GetInteractionPrompt_Implementation() const
{
	return bIsPulled ? FText::FromString(TEXT("Press 'E' to Pull Lever")) : FText::FromString(TEXT("Press 'E' to Pull Lever"));
}