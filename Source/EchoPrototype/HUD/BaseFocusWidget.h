#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseFocusWidget.generated.h"

class UButton;
class UParallaxBackgroundWidget;

UCLASS(Abstract)
class ECHOPROTOTYPE_API UBaseFocusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Focus", meta = (BindWidgetOptional))
	TObjectPtr<UButton> FirstFocusedElement;

private:
	void FocusButton(UButton* Button) const;

	void ApplyInitialFocus();

	UParallaxBackgroundWidget* GetParallaxWidget();

	void UpdateParallaxFocusTarget(const FGeometry& MyGeometry, UButton* FocusedButton);

	void SetGamepadActive(bool bActive);

	TWeakObjectPtr<UButton> LastFocusedButton;
	TWeakObjectPtr<UParallaxBackgroundWidget> CachedParallaxWidget;
	FVector2D CachedStickInput = FVector2D::ZeroVector;
	FTimerHandle InitialFocusTimerHandle;

	bool bGamepadActive = false;
	FVector2D LastMouseScreenPosition = FVector2D::ZeroVector;
	bool bHasLastMouseScreenPosition = false;
};
