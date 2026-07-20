#include "MenuStateBase.h"

#include "../MenuFlowSubsystem.h"
#include "Blueprint/UserWidget.h"
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

	if (bShowsMouseCursor)
	{
		PC->SetInputMode(FInputModeGameAndUI());
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
