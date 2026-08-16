#include "BaseFocusWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "ParallaxBackgroundWidget.h"

void UBaseFocusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!FirstFocusedElement && WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (!FirstFocusedElement)
			{
				if (UButton* Btn = Cast<UButton>(Widget))
				{
					FirstFocusedElement = Btn;
				}
			}
		});
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;

		if (FirstFocusedElement)
		{
			InputMode.SetWidgetToFocus(FirstFocusedElement->TakeWidget());
		}

		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	if (UWorld* World = GetWorld())
	{
		InitialFocusTimerHandle = World->GetTimerManager().SetTimerForNextTick(this, &UBaseFocusWidget::ApplyInitialFocus);
	}
}

void UBaseFocusWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitialFocusTimerHandle);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	Super::NativeDestruct();
}

void UBaseFocusWidget::ApplyInitialFocus()
{
	FocusButton(FirstFocusedElement);
}

void UBaseFocusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!FirstFocusedElement || !WidgetTree)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	const int32 SlateUserIndex = (PC && PC->GetLocalPlayer()) ? PC->GetLocalPlayer()->GetControllerId() : 0;

	const TSharedPtr<SWidget> FocusedSlate = FSlateApplication::Get().GetUserFocusedWidget(SlateUserIndex);

	UButton* FocusedButton = nullptr;
	if (FocusedSlate.IsValid())
	{
		WidgetTree->ForEachWidget([&FocusedButton, &FocusedSlate](UWidget* Widget)
		{
			if (!FocusedButton)
			{
				if (UButton* Btn = Cast<UButton>(Widget))
				{
					if (Btn->GetCachedWidget() == FocusedSlate)
					{
						FocusedButton = Btn;
					}
				}
			}
		});
	}

	if (FocusedButton)
	{
		LastFocusedButton = FocusedButton;
		return;
	}

	FocusButton(LastFocusedButton.IsValid() ? LastFocusedButton.Get() : FirstFocusedElement.Get());
}

FReply UBaseFocusWidget::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	const FReply Reply = Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);

	const FKey Key = InAnalogEvent.GetKey();

	if (Key == EKeys::Gamepad_RightX)
	{
		CachedStickInput.X = InAnalogEvent.GetAnalogValue();
	}
	else if (Key == EKeys::Gamepad_RightY)
	{
		CachedStickInput.Y = InAnalogEvent.GetAnalogValue();
	}
	else
	{
		return Reply;
	}

	if (!CachedParallaxWidget.IsValid() && WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (!CachedParallaxWidget.IsValid())
			{
				if (UParallaxBackgroundWidget* Parallax = Cast<UParallaxBackgroundWidget>(Widget))
				{
					CachedParallaxWidget = Parallax;
				}
			}
		});
	}

	if (UParallaxBackgroundWidget* Parallax = CachedParallaxWidget.Get())
	{
		Parallax->SetGamepadStickInput(CachedStickInput);
	}

	return Reply;
}

void UBaseFocusWidget::FocusButton(UButton* Button) const
{
	if (Button)
	{
		Button->SetUserFocus(GetOwningPlayer());
	}
}
