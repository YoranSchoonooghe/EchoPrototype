#pragma once

#include "CoreMinimal.h"
#include "MenuStateBase.h"
#include "MenuState_Playing.generated.h"

UCLASS()
class ECHOPROTOTYPE_API UMenuState_Playing : public UMenuStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<UMenuStateBase> PausedStateClass;

	virtual void OnBackPressedAtRoot(UMenuFlowSubsystem* Flow) override;
};
