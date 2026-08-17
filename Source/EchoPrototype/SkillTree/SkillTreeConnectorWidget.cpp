#include "SkillTreeConnectorWidget.h"

#include "Rendering/DrawElements.h"
#include "Widgets/SLeafWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogSkillTreeConnectors, Log, All);

struct FSkillTreeConnectorRenderData
{
	TWeakObjectPtr<UWidget> StartWidget;
	TWeakObjectPtr<UWidget> EndWidget;
	FVector2D StartPosition = FVector2D::ZeroVector;
	FVector2D EndPosition = FVector2D::ZeroVector;
	FLinearColor Color = FLinearColor::Gray;
	float Thickness = 3.0f;
};

class SSkillTreeConnectors : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeConnectors) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	void SetConnectors(const TArray<FSkillTreeConnector>& InConnectors)
	{
		Connectors.Reset(InConnectors.Num());

		for (const FSkillTreeConnector& Connector : InConnectors)
		{
			FSkillTreeConnectorRenderData& RenderData = Connectors.AddDefaulted_GetRef();
			RenderData.StartWidget = Connector.StartWidget;
			RenderData.EndWidget = Connector.EndWidget;
			RenderData.StartPosition = Connector.StartPosition;
			RenderData.EndPosition = Connector.EndPosition;
			RenderData.Color = Connector.Color;
			RenderData.Thickness = Connector.Thickness;
		}

		UE_LOG(LogSkillTreeConnectors, Log, TEXT("SSkillTreeConnectors::SetConnectors: %d render entries built (from %d incoming)"), Connectors.Num(), InConnectors.Num());

		bHasLoggedThisPaint = false;
		Invalidate(EInvalidateWidgetReason::Paint);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry();

		for (const FSkillTreeConnectorRenderData& Connector : Connectors)
		{
			const FVector2D Start = GetPointLocal(Connector.StartWidget, Connector.StartPosition, AllottedGeometry);
			const FVector2D End = GetPointLocal(Connector.EndWidget, Connector.EndPosition, AllottedGeometry);

			if (!bHasLoggedThisPaint)
			{
				UE_LOG(LogSkillTreeConnectors, Log, TEXT("SSkillTreeConnectors::OnPaint: connector Start=%s End=%s Color=%s Thickness=%.1f"),
					*Start.ToString(), *End.ToString(), *Connector.Color.ToString(), Connector.Thickness);
			}

			const TArray<FVector2D> Points = { Start, End };

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				PaintGeometry,
				Points,
				ESlateDrawEffect::None,
				Connector.Color,
				true,
				Connector.Thickness
			);
		}

		if (!bHasLoggedThisPaint)
		{
			bHasLoggedThisPaint = true;
			UE_LOG(LogSkillTreeConnectors, Log, TEXT("SSkillTreeConnectors::OnPaint: %d connectors drawn, AllottedGeometry size=%s"),
				Connectors.Num(), *AllottedGeometry.GetLocalSize().ToString());
		}

		return LayerId;
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return FVector2D::ZeroVector;
	}

private:
	static FVector2D GetPointLocal(const TWeakObjectPtr<UWidget>& WeakWidget, const FVector2D& FallbackPosition, const FGeometry& AllottedGeometry)
	{
		if (UWidget* Widget = WeakWidget.Get())
		{
			const FGeometry& WidgetGeometry = Widget->GetCachedGeometry();
			if (!WidgetGeometry.GetAbsoluteSize().IsNearlyZero())
			{
				const FVector2D AbsoluteCenter = WidgetGeometry.GetAbsolutePosition() + WidgetGeometry.GetAbsoluteSize() * 0.5f;
				return FVector2D(AllottedGeometry.AbsoluteToLocal(AbsoluteCenter));
			}
		}

		return FallbackPosition;
	}

	TArray<FSkillTreeConnectorRenderData> Connectors;
	mutable bool bHasLoggedThisPaint = false;
};

TSharedRef<SWidget> USkillTreeConnectorWidget::RebuildWidget()
{
	MySlateWidget = SNew(SSkillTreeConnectors);
	return MySlateWidget.ToSharedRef();
}

void USkillTreeConnectorWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MySlateWidget.Reset();
}

void USkillTreeConnectorWidget::SetConnectors(const TArray<FSkillTreeConnector>& InConnectors)
{
	UE_LOG(LogSkillTreeConnectors, Log, TEXT("%s::SetConnectors: %d connectors, MySlateWidget valid=%s"),
		*GetName(), InConnectors.Num(), MySlateWidget.IsValid() ? TEXT("true") : TEXT("false"));

	if (MySlateWidget.IsValid())
	{
		MySlateWidget->SetConnectors(InConnectors);
	}
}
