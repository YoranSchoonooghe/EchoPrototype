#pragma once

#include "CoreMinimal.h"
#include "EchoTypes.generated.h"

UENUM(BlueprintType)
enum class EEchoType : uint8
{
	Teleport,
	Vision
};