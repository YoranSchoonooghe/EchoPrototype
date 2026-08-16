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

	if (!FocusedButton)
	{
		FocusedButton = LastFocusedButton.IsValid() ? LastFocusedButton.Get() : FirstFocusedElement.Get();
		FocusButton(FocusedButton);
	}

	if (FocusedButton != LastFocusedButton.Get())
	{
		LastFocusedButton = FocusedButton;
		UpdateParallaxFocusTarget(MyGeometry, FocusedButton);
	}
}

FReply UBaseFocusWidget::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	const FReply Reply = Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);

	const FKey Key = InAnalogEvent.GetKey();

	if (Key.IsGamepadKey() && FMath::Abs(InAnalogEvent.GetAnalogValue()) > 0.1f)
	{
		SetGamepadActive(true);
	}

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

	if (UParallaxBackgroundWidget* Parallax = GetParallaxWidget())
	{
		Parallax->SetGamepadStickInput(CachedStickInput);
	}

	return Reply;
}

FReply UBaseFocusWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FVector2D ScreenPos = InMouseEvent.GetScreenSpacePosition();

	const bool bRealMovement = !bHasLastMouseScreenPosition
		|| FVector2D::DistSquared(ScreenPos, LastMouseScreenPosition) > 4.f; // ~2px

	LastMouseScreenPosition = ScreenPos;
	bHasLastMouseScreenPosition = true;

	if (bRealMovement)
	{
		SetGamepadActive(false);
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UBaseFocusWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey().IsGamepadKey())
	{
		SetGamepadActive(true);
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBaseFocusWidget::FocusButton(UButton* Button) const
{
	if (Button)
	{
		Button->SetUserFocus(GetOwningPlayer());
	}
}

UParallaxBackgroundWidget* UBaseFocusWidget::GetParallaxWidget()
{
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

	return CachedParallaxWidget.Get();
}

void UBaseFocusWidget::UpdateParallaxFocusTarget(const FGeometry& MyGeometry, UButton* FocusedButton)
{
	UParallaxBackgroundWidget* Parallax = GetParallaxWidget();
	if (!Parallax)
	{
		return;
	}

	if (!FocusedButton)
	{
		Parallax->ClearFocusedElementOffset();
		return;
	}

	const FGeometry& ButtonGeometry = FocusedButton->GetCachedGeometry();
	const FVector2D ButtonAbsoluteCenter = ButtonGeometry.GetAbsolutePosition() + ButtonGeometry.GetAbsoluteSize() * 0.5f;
	const FVector2D LocalCenter = MyGeometry.AbsoluteToLocal(ButtonAbsoluteCenter);
	const FVector2D LocalSize = MyGeometry.GetLocalSize();

	if (LocalSize.X <= 0.f || LocalSize.Y <= 0.f)
	{
		return;
	}

	const FVector2D Normalized(
		FMath::Clamp((LocalCenter.X / LocalSize.X) * 2.f - 1.f, -1.f, 1.f),
		FMath::Clamp((LocalCenter.Y / LocalSize.Y) * 2.f - 1.f, -1.f, 1.f));

	Parallax->SetFocusedElementOffset(Normalized);
}

void UBaseFocusWidget::SetGamepadActive(bool bActive)
{
	if (bGamepadActive == bActive)
	{
		return;
	}

	bGamepadActive = bActive;

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(!bGamepadActive);
	}

	if (UParallaxBackgroundWidget* Parallax = GetParallaxWidget())
	{
		Parallax->SetMouseInputSuppressed(bGamepadActive);
	}
}
