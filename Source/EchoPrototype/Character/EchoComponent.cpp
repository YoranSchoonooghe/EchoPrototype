// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoComponent.h"

#include "../Echo/EchoActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UEchoComponent::UEchoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEchoComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* OwnerPawn = GetOwnerPawn())
	{
		Camera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	}
}


// Called every frame
void UEchoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EchoState == EEchoState::Aiming)
	{
		UpdateAimPreview(DeltaTime);
	}

	if (FovEffect == EEchoFOVEffect::ZoomingIn)
	{
		UpdateTeleportFovEffect(DeltaTime);
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

		if (bCurrentAimIsValid)
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
	if (HeldDuration > TapThreshold && bCurrentAimIsValid)
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

	if (bIsViewingThroughEcho)
	{
		ReturnViewToSelf(); 
		return;
	}

	if (APawn* OwnerPawn = GetOwnerPawn())
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PC->SetViewTargetWithBlend(ActiveEcho, ViewBlendTime);
			bIsViewingThroughEcho = true;

			if (EchoVisionPostProcessMaterial)
			{
				if (UCameraComponent* EchoCam = ActiveEcho->GetEchoCamera())
				{
					EchoCam->PostProcessSettings.AddBlendable(EchoVisionPostProcessMaterial, 1.0f);
				}
			}
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

			if (EchoVisionPostProcessMaterial && ActiveEcho)
			{
				if (UCameraComponent* EchoCam = ActiveEcho->GetEchoCamera())
				{
					EchoCam->PostProcessSettings.RemoveBlendable(EchoVisionPostProcessMaterial);
				}
			}
		}
	}
	bIsViewingThroughEcho = false;
}

void UEchoComponent::AddEchoLookInput(float YawDelta, float PitchDelta)
{
	if (ActiveEcho && bIsViewingThroughEcho)
	{
		ActiveEcho->AddCameraLookInput(YawDelta, PitchDelta);
	}
}

void UEchoComponent::AddEchoMoveInput(const FVector2D& MoveInput)
{
	if (!ActiveEcho || !bIsViewingThroughEcho || MoveInput.IsNearlyZero())
	{
		return;
	}

	UCameraComponent* EchoCam = ActiveEcho->GetEchoCamera();
	if (!EchoCam)
	{
		return;
	}

	const FRotator YawOnly(0.0f, EchoCam->GetComponentRotation().Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y);

	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	const FVector WorldDelta = (Forward * MoveInput.Y + Right * MoveInput.X) * EchoMoveSpeed * DeltaSeconds;

	ActiveEcho->AddMoveInput(WorldDelta);
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

	FVector TargetLocation = ActiveEcho->GetActorLocation();
	const FRotator TargetRotation = ActiveEcho->GetActorRotation();

	if (UCapsuleComponent* CharacterCapsule = OwnerCharacter->GetCapsuleComponent())
	{
		TargetLocation.Z += CharacterCapsule->GetScaledCapsuleHalfHeight();
	}

	StartTeleportFovEffect();


	if (!OwnerCharacter->TeleportTo(TargetLocation, TargetRotation, true, false))
	{
		if (Camera)
		{
			Camera->SetFieldOfView(FovEffectBaseFOV);
		}
		FovEffect = EEchoFOVEffect::None;

		return;
	}

	ReturnViewToSelf();
	DestroyActiveEcho();
}

#pragma endregion input


void UEchoComponent::StartTeleportFovEffect()
{
	if (!Camera) return;


	FovEffectBaseFOV = Camera->FieldOfView;

	FovEffectStartFOV = TeleportFOV;

	Camera->SetFieldOfView(TeleportFOV);
	FovEffectElapsed = 0.0f;

	FovEffect = EEchoFOVEffect::ZoomingIn;
}

void UEchoComponent::UpdateTeleportFovEffect(float DeltaTime)
{
	if (!Camera)
	{
		FovEffect = EEchoFOVEffect::None;
		return;
	}

	FovEffectElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(FovEffectElapsed / TeleportZoomDuration, 0.0f, 1.0f);

	const float Eased = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);
	Camera->SetFieldOfView(FMath::Lerp(FovEffectStartFOV, FovEffectBaseFOV, Eased));

	if (Alpha >= 1.0f)
	{
		Camera->SetFieldOfView(FovEffectBaseFOV);
		FovEffect = EEchoFOVEffect::None;
	}
}


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
	bCurrentAimIsValid = bValid;
	LastAimRotation = SpawnRotation;

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
	bCurrentAimIsValid = bValid;
	LastAimRotation = TargetRotation;

	TargetRotation.Pitch = 0.0f;

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

	FVector FinalLocation;
	FRotator FinalRotation;
	bool bValid = false;
	TraceForEchoLocation(FinalLocation, FinalRotation, bValid);

	if (bValid)
	{
		FinalRotation.Pitch = 0.0f;
		FinalRotation.Roll = 0.0f;

		const float VerticalSpawnBuffer = 10.0f;
		FinalLocation.Z += VerticalSpawnBuffer;

		ActiveEcho->SetActorLocationAndRotation(FinalLocation, FinalRotation);


		ActiveEcho->SetVisualState(EEchoVisualState::Placed);
		ActiveEcho->InitializeCameraFacing(LastAimRotation);
		EchoState = EEchoState::Placed;
	}
}

void UEchoComponent::DestroyActiveEcho()
{
	if (ActiveEcho)
	{
		ActiveEcho->Destroy();
		ActiveEcho = nullptr;
	}
	EchoState = EEchoState::Idle;
	bIsViewingThroughEcho = false;
	bCurrentAimIsValid = false;
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
	OutRotation = CamRot;

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * EchoTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (ActiveEcho)
	{
		QueryParams.AddIgnoredActor(ActiveEcho);
	}

	//Forward sweep along aim direction
		FHitResult ForwardHit;
	const bool bForwardHit = GetWorld()->SweepSingleByChannel(
		ForwardHit, CamLoc, TraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(EchoTraceRadius), QueryParams
	);

	if (!bForwardHit)
	{
		//Nothing in range
		OutLocation = TraceEnd;
		bOutValid = false;
		return true;
	}


	if (ForwardHit.ImpactNormal.Z >= MinWalkableNormalZ)
	{
		OutLocation = ForwardHit.ImpactPoint + (ForwardHit.ImpactNormal * GroundPlacementOffset);
		bOutValid = true;
		return true;
	}

	//If forward hit was a wall, search straight down
	const FVector DownTraceStart = ForwardHit.ImpactPoint + FVector(0.0f, 0.0f, GroundSearchHeight);
	const FVector DownTraceEnd = ForwardHit.ImpactPoint - FVector(0.0f, 0.0f, GroundSearchDepth);

	FHitResult DownHit;
	const bool bDownHit = GetWorld()->SweepSingleByChannel(
		DownHit, DownTraceStart, DownTraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(EchoTraceRadius), QueryParams
	);

	if (bDownHit && DownHit.ImpactNormal.Z >= MinWalkableNormalZ)
	{
		OutLocation = DownHit.ImpactPoint + (DownHit.ImpactNormal * GroundPlacementOffset);
		bOutValid = true;
	}
	else
	{
		OutLocation = ForwardHit.ImpactPoint;
		bOutValid = false;
	}

	return true;
}