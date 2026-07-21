#include "SkillTreeConnectorWidget.h"

#include "Rendering/DrawElements.h"
#include "Widgets/SLeafWidget.h"

class SSkillTreeConnectors : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeConnectors) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs) {}

	void SetConnectors(const TArray<FSkillTreeConnector>& InConnectors)
	{
		Connectors = InConnectors;
		Invalidate(EInvalidateWidgetReason::Paint);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry();

		for (const FSkillTreeConnector& Connector : Connectors)
		{
			const TArray<FVector2D> Points = { Connector.Start, Connector.End };

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

		return LayerId;
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return FVector2D::ZeroVector;
	}

private:
	TArray<FSkillTreeConnector> Connectors;
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
	if (MySlateWidget.IsValid())
	{
		MySlateWidget->SetConnectors(InConnectors);
	}
}
