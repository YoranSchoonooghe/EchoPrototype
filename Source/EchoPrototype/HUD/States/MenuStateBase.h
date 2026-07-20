#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MenuStateBase.generated.h"

class UMenuFlowSubsystem;
class UUserWidget;

UCLASS(Abstract, Blueprintable)
class ECHOPROTOTYPE_API UMenuStateBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	bool bPausesGame = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	bool bShowsMouseCursor = false;

	virtual void EnterState(UMenuFlowSubsystem* Flow);
	virtual void ExitState(UMenuFlowSubsystem* Flow);
	virtual void OnBackPressedAtRoot(UMenuFlowSubsystem* Flow) {}

protected:
	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveWidget;
};
