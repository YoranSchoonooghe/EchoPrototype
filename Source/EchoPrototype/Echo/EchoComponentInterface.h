// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EchoComponentInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEchoComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ECHOPROTOTYPE_API IEchoComponentInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Echo")
	bool GetDetectability() const;
};
