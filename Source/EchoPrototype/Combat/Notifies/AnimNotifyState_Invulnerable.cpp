// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_Invulnerable.h"
#include "../DodgeComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_Invulnerable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UDodgeComponent* Dodge = Owner->FindComponentByClass<UDodgeComponent>())
		{
			Dodge->BeginInvulnerability();
		}
	}
}

void UAnimNotifyState_Invulnerable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UDodgeComponent* Dodge = Owner->FindComponentByClass<UDodgeComponent>())
		{
			Dodge->EndInvulnerability();
		}
	}
}
