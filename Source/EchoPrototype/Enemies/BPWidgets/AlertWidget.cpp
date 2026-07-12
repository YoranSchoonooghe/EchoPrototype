#include "AlertWidget.h"
#include "Components/Image.h"

void UAlertWidget::UpdateAlertIcon(EAlertState alertState)
{
	switch (alertState)
	{
	case EAlertState::Neutral:
		SetVisibility(ESlateVisibility::Hidden);
		break;
	case EAlertState::Suspicious:
		SetVisibility(ESlateVisibility::Visible);
		Icon->SetBrushFromTexture(SuspiciousTexture);
		break;
	case EAlertState::Alert:
		SetVisibility(ESlateVisibility::Visible);
		Icon->SetBrushFromTexture(AlertTexture);
		break;
	}

}
