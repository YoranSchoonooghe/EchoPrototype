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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Focus", meta = (BindWidgetOptional))
	TObjectPtr<UButton> FirstFocusedElement;

private:
	void FocusButton(UButton* Button) const;

	void ApplyInitialFocus();

	TWeakObjectPtr<UButton> LastFocusedButton;
	TWeakObjectPtr<UParallaxBackgroundWidget> CachedParallaxWidget;
	FVector2D CachedStickInput = FVector2D::ZeroVector;
	FTimerHandle InitialFocusTimerHandle;
};
