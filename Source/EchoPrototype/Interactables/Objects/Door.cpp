// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

// Sets default values
ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(DoorMesh);
	DoorMesh->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedRotation = GetActorRotation();
	OpenRotation = ClosedRotation + FRotator(0.0f, OpenYawAngle, 0.0f);
	TargetRotation = ClosedRotation;
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DoorState != EDoorState::Opening && DoorState != EDoorState::Closing)
	{
		return;
	}

	const FRotator NewRotation = FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, DeltaTime, SwingSpeed);
	SetActorRotation(NewRotation);

	if (NewRotation.Equals(TargetRotation, 0.5f))
	{
		SetActorRotation(TargetRotation);
		DoorState = (DoorState == EDoorState::Opening) ? EDoorState::Open : EDoorState::Closed;
	}
}

void ADoor::OpenDoor()
{
	if (DoorState == EDoorState::Open || DoorState == EDoorState::Opening)
	{
		return;
	}

	TargetRotation = OpenRotation;
	DoorState = EDoorState::Opening;
}

void ADoor::CloseDoor()
{
	if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
	{
		return;
	}

	TargetRotation = ClosedRotation;
	DoorState = EDoorState::Closing;
}

void ADoor::ToggleDoor()
{
	const bool bTreatAsOpen = (DoorState == EDoorState::Open || DoorState == EDoorState::Opening);
	bTreatAsOpen ? CloseDoor() : OpenDoor();
}