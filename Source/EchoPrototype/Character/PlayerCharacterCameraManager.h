#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PlayerCharacterCameraManager.generated.h"

UCLASS()
class ECHOPROTOTYPE_API APlayerCharacterCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	APlayerCharacterCameraManager();
};
