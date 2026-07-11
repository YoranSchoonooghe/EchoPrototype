// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WeaponTrace.generated.h"

UCLASS(meta = (DisplayName = "Weapon Trace"))
class ECHOPROTOTYPE_API UAnimNotifyState_WeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon Trace")
	FName WeaponSocketName = "WeaponTip";

	UPROPERTY(EditAnywhere, Category = "Weapon Trace")
	float TraceRadius = 40.0f;

	// -1 = use the current combo attack's DamageAmount from the CombatComponent.
	UPROPERTY(EditAnywhere, Category = "Weapon Trace")
	float DamageOverride = -1.0f;
};
