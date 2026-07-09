// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoComponent.h"

#include "../Echo/EchoActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UEchoComponent::UEchoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEchoComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UEchoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EchoState == EEchoState::Aiming)
	{
		UpdateAimPreview(DeltaTime);
	}
}

APawn* UEchoComponent::GetOwnerPawn() const
{
	return Cast<APawn>(GetOwner());
}

#pragma region input

void UEchoComponent::OnEchoPressed()
{
	PressStartTime = GetWorld()->GetTimeSeconds();

	switch (EchoState)
	{
	case EEchoState::Idle:
		BeginAiming();
		break;
	case EEchoState::Aiming:
		PlaceEcho();
		break;
	case EEchoState::Placed:
		TeleportToEcho();
		break;
	}
}

void UEchoComponent::OnEchoReleased()
{
	if (EchoState != EEchoState::Aiming)
	{
		return;
	}

	const double HeldDuration = GetWorld()->GetTimeSeconds() - PressStartTime;
	if (HeldDuration > TapThreshold)
	{
		PlaceEcho();
	}
}

void UEchoComponent::LookThroughEcho()
{
	if (EchoState != EEchoState::Placed || !ActiveEcho)
	{
		return;
	}

	if (APawn* OwnerPawn = GetOwnerPawn())
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PC->SetViewTargetWithBlend(ActiveEcho, ViewBlendTime);
		}
	}
}

void UEchoComponent::ReturnViewToSelf()
{
	if (APawn* OwnerPawn = GetOwnerPawn())
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PC->SetViewTargetWithBlend(OwnerPawn, ViewBlendTime);
		}
	}
}

void UEchoComponent::TeleportToEcho()
{
	if (EchoState != EEchoState::Placed || !ActiveEcho)
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	const FVector TargetLocation = ActiveEcho->GetActorLocation();
	const FRotator TargetRotation = ActiveEcho->GetActorRotation();

	if (!OwnerCharacter->TeleportTo(TargetLocation, TargetRotation, false, false))
	{
		return;
	}

	ReturnViewToSelf();
	DestroyActiveEcho();
}

#pragma endregion input

void UEchoComponent::BeginAiming()
{
	if (!EchoActorClass)
	{
		return;
	}

	FVector SpawnLocation;
	FRotator SpawnRotation;
	bool bValid = false;
	TraceForEchoLocation(SpawnLocation, SpawnRotation, bValid);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveEcho = GetWorld()->SpawnActor<AEchoActor>(EchoActorClass, FTransform(SpawnRotation, SpawnLocation), SpawnParams);
	if (ActiveEcho)
	{
		ActiveEcho->SetVisualState(EEchoVisualState::Preview);
		ActiveEcho->SetPreviewValidity(bValid);
		EchoState = EEchoState::Aiming;
	}
}

void UEchoComponent::UpdateAimPreview(float DeltaSeconds)
{
	if (!ActiveEcho)
	{
		EchoState = EEchoState::Idle;
		return;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	bool bValid = false;
	TraceForEchoLocation(TargetLocation, TargetRotation, bValid);

	const FVector NewLocation = FMath::VInterpTo(ActiveEcho->GetActorLocation(), TargetLocation, DeltaSeconds, PreviewInterpSpeed);
	ActiveEcho->SetActorLocation(NewLocation);
	ActiveEcho->SetActorRotation(TargetRotation);
	ActiveEcho->SetPreviewValidity(bValid);
}

void UEchoComponent::PlaceEcho()
{
	if (!ActiveEcho)
	{
		return;
	}

	ActiveEcho->SetVisualState(EEchoVisualState::Placed);
	EchoState = EEchoState::Placed;
}

void UEchoComponent::DestroyActiveEcho()
{
	if (ActiveEcho)
	{
		ActiveEcho->Destroy();
		ActiveEcho = nullptr;
	}
	EchoState = EEchoState::Idle;
}


bool UEchoComponent::TraceForEchoLocation(FVector& OutLocation, FRotator& OutRotation, bool& bOutValid) const
{
	APawn* OwnerPawn = GetOwnerPawn();
	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (!PC)
	{
		OutLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
		OutRotation = FRotator::ZeroRotator;
		bOutValid = false;
		return false;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * EchoTraceDistance);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (ActiveEcho)
	{
		QueryParams.AddIgnoredActor(ActiveEcho);
	}

	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		CamLoc,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(EchoTraceRadius),
		QueryParams
	);

	OutRotation = CamRot;
	if (bHit)
	{
		OutLocation = Hit.ImpactPoint + (Hit.ImpactNormal * EchoTraceRadius);
	}
	else
	{
		OutLocation = TraceEnd;
	}
	bOutValid = true;

	return true;
}