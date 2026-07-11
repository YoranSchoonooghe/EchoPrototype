// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_WeaponTrace.h"
#include "../CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UCombatComponent* Combat = Owner->FindComponentByClass<UCombatComponent>())
		{
			Combat->BeginWeaponTrace(WeaponSocketName, TraceRadius, DamageOverride);
		}
	}
}

void UAnimNotifyState_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UCombatComponent* Combat = Owner->FindComponentByClass<UCombatComponent>())
		{
			Combat->UpdateWeaponTrace();
		}
	}
}

void UAnimNotifyState_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UCombatComponent* Combat = Owner->FindComponentByClass<UCombatComponent>())
		{
			Combat->EndWeaponTrace();
		}
	}
}
