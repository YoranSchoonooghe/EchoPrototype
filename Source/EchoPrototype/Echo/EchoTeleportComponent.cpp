// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoTeleportComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

#include "NiagaraFunctionLibrary.h"


UEchoTeleportComponent::UEchoTeleportComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEchoTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsRecovering)
	{
		UpdateTeleportEffects(DeltaTime);
	}
}

bool UEchoTeleportComponent::ExecuteTeleport(APawn* PlayerPawn)
{
	AActor* EchoActor = GetOwner();
	if (!PlayerPawn || !EchoActor)
	{
		return false;
	}

	CachedPlayerPawn = PlayerPawn;

	StartTeleportEffects(PlayerPawn);

	OnTeleportStarted.Broadcast(Cast<AEchoCharacter>(GetOwner()));
	PlayTeleportDepartureSound();

	if (DepartureFX)
	{
		FVector DepartureLoc = PlayerPawn->GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DepartureFX, DepartureLoc);

	}

	SetPlayerVisible(PlayerPawn, false);

	if (PreTeleportDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TeleportTimerHandle,
			this,
			&UEchoTeleportComponent::PerformActualTeleport,
			PreTeleportDelay,
			false
		);
	}
	else
	{
		PerformActualTeleport();
	}

	return true;
}

void UEchoTeleportComponent::PerformActualTeleport()
{
	APawn* PlayerPawn = CachedPlayerPawn.Get();
	AActor* EchoActor = GetOwner();

	if (!PlayerPawn || !EchoActor)
	{
		ResetPostProcessing();
		bIsRecovering = false;
		OnTeleportComplete.Broadcast(Cast<AEchoCharacter>(GetOwner()));
		PlayTeleportArrivalSound();
		return;
	}

	const FVector TargetLocation = EchoActor->GetActorLocation();
	const FRotator TargetRotation = PlayerPawn->GetActorRotation();

	//Disable the ORB collision
	UCapsuleComponent* EchoCapsule = EchoActor->FindComponentByClass<UCapsuleComponent>();
	if (EchoCapsule)
	{
		EchoCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//Teleport player
	const bool bSuccess = PlayerPawn->TeleportTo(TargetLocation, TargetRotation, false, true);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("TeleportTo failed! Overriding with SetActorLocation..."));

		PlayerPawn->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	//Trigger Camera Shake on Arrival
	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (PC && ArrivalCameraShakeClass)
	{
		PC->ClientStartCameraShake(ArrivalCameraShakeClass, 1.0f);
	}

	if (ArrivalFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ArrivalFX, TargetLocation);
	}

	SetPlayerVisible(PlayerPawn, true);

	EffectElapsed = 0.0f;
	bIsRecovering = true;
}

void UEchoTeleportComponent::StartTeleportEffects(APawn* PlayerPawn)
{
	if (!PlayerPawn) return;

	UCameraComponent* Cam = PlayerPawn->FindComponentByClass<UCameraComponent>();
	if (!Cam) return;

	FovEffectCamera = Cam;
	FovEffectBaseFOV = Cam->FieldOfView;
	FovEffectStartFOV = TeleportSpikeFOV;

	Cam->SetFieldOfView(TeleportSpikeFOV);

	FPostProcessSettings& PP = Cam->PostProcessSettings;
	PP.bOverride_SceneFringeIntensity = true;
	PP.SceneFringeIntensity = MaxChromaticAberration;

	PP.bOverride_MotionBlurAmount = true;
	PP.MotionBlurAmount = MaxMotionBlur;

	bIsRecovering = false;
}

void UEchoTeleportComponent::UpdateTeleportEffects(float DeltaSeconds)
{
	if (!FovEffectCamera.IsValid() || TeleportRecoveryDuration <= 0.0f)
	{
		ResetPostProcessing();
		bIsRecovering = false;
		FovEffectCamera.Reset();
		OnTeleportComplete.Broadcast(Cast<AEchoCharacter>(GetOwner()));
		PlayTeleportArrivalSound();
		return;
	}

	EffectElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(EffectElapsed / TeleportRecoveryDuration, 0.0f, 1.0f);

	//Fast cubic ease-out curve
	const float Eased = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);

	//Interpolate FOV
	FovEffectCamera->SetFieldOfView(FMath::Lerp(FovEffectStartFOV, FovEffectBaseFOV, Eased));

	//Decay Post-Processing
	FPostProcessSettings& PP = FovEffectCamera->PostProcessSettings;
	PP.SceneFringeIntensity = FMath::Lerp(MaxChromaticAberration, 0.0f, Eased);
	PP.MotionBlurAmount = FMath::Lerp(MaxMotionBlur, 0.0f, Eased);

	if (Alpha >= 1.0f)
	{
		ResetPostProcessing();
		bIsRecovering = false;
		FovEffectCamera.Reset();
		OnTeleportComplete.Broadcast(Cast<AEchoCharacter>(GetOwner()));
		PlayTeleportArrivalSound();
	}
}

void UEchoTeleportComponent::ResetPostProcessing()
{
	if (FovEffectCamera.IsValid())
	{
		FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);

		FPostProcessSettings& PP = FovEffectCamera->PostProcessSettings;
		PP.bOverride_SceneFringeIntensity = false;
		PP.SceneFringeIntensity = 0.0f;

		PP.bOverride_MotionBlurAmount = false;
		PP.MotionBlurAmount = 0.0f;
	}
}

void UEchoTeleportComponent::SetPlayerVisible(APawn* PlayerPawn, bool bVisible)
{
	if (!PlayerPawn) return;

	USkeletalMeshComponent* Mesh = PlayerPawn->FindComponentByClass<USkeletalMeshComponent>();
	if (Mesh)
	{
		Mesh->SetVisibility(bVisible);
	}
}
