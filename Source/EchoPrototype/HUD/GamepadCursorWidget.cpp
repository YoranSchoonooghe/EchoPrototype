#include "GamepadCursorWidget.h"
#include "Framework/Application/AnalogCursor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/IInputProcessor.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/PlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogGamepadCursor, Log, All);

class FGamepadKeyButtonInputProcessor : public IInputProcessor
{
public:
	FGamepadKeyButtonInputProcessor(TFunction<void(FKey)> InOnKeyPressed)
		: OnKeyPressed(MoveTemp(InOnKeyPressed))
	{
	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.IsRepeat())
		{
			return false;
		}

		if (OnKeyPressed)
		{
			OnKeyPressed(InKeyEvent.GetKey());
		}

		return false;
	}

	virtual const TCHAR* GetDebugName() const override
	{
		return TEXT("GamepadKeyButtonInputProcessor");
	}

private:
	TFunction<void(FKey)> OnKeyPressed;
};

void UGamepadCursorWidget::RegisterKeyButtonBinding(FKey Key, UButton* Button)
{
	if (!Button)
	{
		return;
	}

	FGamepadKeyButtonBinding& Binding = KeyButtonBindings.AddDefaulted_GetRef();
	Binding.Key = Key;
	Binding.Button = Button;
}

void UGamepadCursorWidget::ClearKeyButtonBindings()
{
	KeyButtonBindings.Reset();
}

void UGamepadCursorWidget::HandleBoundKeyPressed(FKey Key)
{
	for (const FGamepadKeyButtonBinding& Binding : KeyButtonBindings)
	{
		if (Binding.Key == Key && Binding.Button)
		{
			Binding.Button->OnClicked.Broadcast();
		}
	}
}

void UGamepadCursorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WidgetTree)
	{
		for (FGamepadKeyButtonBinding& Binding : KeyButtonBindings)
		{
			if (!Binding.Button && !Binding.ButtonName.IsNone())
			{
				Binding.Button = WidgetTree->FindWidget<UButton>(Binding.ButtonName);

				if (!Binding.Button)
				{
					UE_LOG(LogGamepadCursor, Warning, TEXT("%s: KeyButtonBindings entry for key %s references ButtonName '%s', but no Button with that name was found in this widget's tree"),
						*GetName(), *Binding.Key.ToString(), *Binding.ButtonName.ToString());
				}
			}
		}
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	if (!AnalogCursor.IsValid())
	{
		AnalogCursor = MakeShared<FAnalogCursor>();
		AnalogCursor->SetAcceleration(CursorAcceleration);
		AnalogCursor->SetMaxSpeed(CursorMaxSpeed);
		AnalogCursor->SetDeadZone(CursorDeadZone);

		FSlateApplication::Get().RegisterInputPreProcessor(AnalogCursor);
		UE_LOG(LogGamepadCursor, Log, TEXT("%s: FAnalogCursor registered"), *GetName());
	}

	if (!KeyButtonInputProcessor.IsValid())
	{
		KeyButtonInputProcessor = MakeShared<FGamepadKeyButtonInputProcessor>([this](FKey Key)
		{
			HandleBoundKeyPressed(Key);
		});

		FSlateApplication::Get().RegisterInputPreProcessor(KeyButtonInputProcessor);
		UE_LOG(LogGamepadCursor, Log, TEXT("%s: FGamepadKeyButtonInputProcessor registered"), *GetName());
	}
}

void UGamepadCursorWidget::NativeDestruct()
{
	if (AnalogCursor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(AnalogCursor);
		AnalogCursor.Reset();
	}

	if (KeyButtonInputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(KeyButtonInputProcessor);
		KeyButtonInputProcessor.Reset();
	}

	Super::NativeDestruct();
}
