#include "ParallaxBackgroundWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBox.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UParallaxBackgroundWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	BuildLayers();
}

void UParallaxBackgroundWidget::BuildLayers()
{
	LayerImages.Empty();

	if (!LayerContainer)
	{
		return;
	}

	LayerContainer->ClearChildren();

	for (const FParallaxLayer& Layer : Layers)
	{
		UScaleBox* LayerScaleBox = NewObject<UScaleBox>(this);
		LayerScaleBox->SetStretch(EStretch::ScaleToFill);

		UImage* LayerImage = NewObject<UImage>(this);
		if (Layer.Image)
		{
			LayerImage->SetBrushFromTexture(Layer.Image, true);
		}
		LayerImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		LayerImage->SetRenderScale(FVector2D(Layer.OversizeScale, Layer.OversizeScale));

		LayerScaleBox->AddChild(LayerImage);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(LayerContainer->AddChild(LayerScaleBox)))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
		}

		LayerImages.Add(LayerImage);
	}
}

FReply UParallaxBackgroundWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bMouseInputSuppressed)
	{
		return FReply::Unhandled();
	}

	const FVector2D LocalMouse = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D LocalSize = InGeometry.GetLocalSize();

	if (LocalSize.X > 0.f && LocalSize.Y > 0.f)
	{
		const FVector2D Normalized(
			FMath::Clamp((LocalMouse.X / LocalSize.X) * 2.f - 1.f, -1.f, 1.f),
			FMath::Clamp((LocalMouse.Y / LocalSize.Y) * 2.f - 1.f, -1.f, 1.f));

		MouseTargetOffset = Normalized * MaxOffsetPixels;

		bHasFocusTarget = false;
	}

	return FReply::Unhandled();
}

void UParallaxBackgroundWidget::SetGamepadStickInput(const FVector2D& StickValue)
{
	bStickActive = StickValue.SizeSquared() > FMath::Square(StickDeadzone);

	if (bStickActive)
	{
		StickTargetOffset = FVector2D(StickValue.X, -StickValue.Y) * MaxOffsetPixels;
	}
}

void UParallaxBackgroundWidget::SetFocusedElementOffset(const FVector2D& NormalizedPosition)
{
	FocusTargetOffset = NormalizedPosition * MaxOffsetPixels;
	bHasFocusTarget = true;
}

void UParallaxBackgroundWidget::ClearFocusedElementOffset()
{
	bHasFocusTarget = false;
}

void UParallaxBackgroundWidget::SetMouseInputSuppressed(bool bSuppressed)
{
	bMouseInputSuppressed = bSuppressed;
}

void UParallaxBackgroundWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FVector2D TargetOffset;
	if (bStickActive)
	{
		TargetOffset = StickTargetOffset;
	}
	else if (bHasFocusTarget)
	{
		TargetOffset = FocusTargetOffset;
	}
	else
	{
		TargetOffset = MouseTargetOffset;
	}

	CurrentOffset = FMath::Vector2DInterpTo(CurrentOffset, TargetOffset, InDeltaTime, InterpSpeed);

	for (int32 Index = 0; Index < LayerImages.Num() && Index < Layers.Num(); ++Index)
	{
		if (LayerImages[Index])
		{
			LayerImages[Index]->SetRenderTranslation(CurrentOffset * Layers[Index].ParallaxStrength);
		}
	}
}
