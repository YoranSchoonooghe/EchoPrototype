#include "MenuStateBase.h"

#include "../MenuFlowSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "../BaseFocusWidget.h"
#include "../GamepadCursorWidget.h"
#include "GameFramework/PlayerController.h"

void UMenuStateBase::EnterState(UMenuFlowSubsystem* Flow)
{
	APlayerController* PC = Flow ? Flow->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}

	if (WidgetClass)
	{
		ActiveWidget = CreateWidget<UUserWidget>(PC, WidgetClass);
		if (ActiveWidget)
		{
			ActiveWidget->AddToViewport();
		}
	}

	PC->SetPause(bPausesGame);
	PC->SetShowMouseCursor(bShowsMouseCursor);

	if (Cast<UBaseFocusWidget>(ActiveWidget) || Cast<UGamepadCursorWidget>(ActiveWidget))
	{
		return;
	}

	if (bShowsMouseCursor)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
	}
	else
	{
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UMenuStateBase::ExitState(UMenuFlowSubsystem* Flow)
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}
}
