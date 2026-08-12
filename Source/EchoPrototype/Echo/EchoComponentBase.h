// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoComponentInterface.h"
#include "EchoComponentBase.generated.h"

UCLASS(Abstract)
class ECHOPROTOTYPE_API UEchoComponentBase : public UActorComponent, public IEchoComponentInterface
{
	GENERATED_BODY()

public:
	virtual bool GetDetectability_Implementation() const override { return bIsDetectable; }

protected:
	UPROPERTY(EditAnywhere, Category = "Echo")
	bool bIsDetectable = false;
};
