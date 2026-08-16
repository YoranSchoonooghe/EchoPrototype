#include "DetectionMeterWidget.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "EchoPrototype/Enemies/EnemyAIController.h"
#include "AIController.h"

void UDetectionMeterWidget::SetEnemy(AEnemyCharacter* Enemy)
{
	_enemyController = Cast<AEnemyAIController>(Enemy->GetController());

	_enemyController->OnDetectionValueChanged.AddDynamic(this, &UDetectionMeterWidget::UpdateVisibility);
}

void UDetectionMeterWidget::SetHidden(bool hidden)
{
	if (hidden)
		SetVisibility(ESlateVisibility::Hidden);
	else
		SetVisibility(ESlateVisibility::Visible);
}

float UDetectionMeterWidget::GetDetectionValue() const
{
	if (!_enemyController) return 0.0f;

	return _enemyController->GetDetectionValue();
}

void UDetectionMeterWidget::UpdateVisibility(float DetectionValue)
{
	if (GetVisibility() == ESlateVisibility::Hidden && DetectionValue > 0.0f)
		SetHidden(false);
	else if (GetVisibility() == ESlateVisibility::Visible && DetectionValue <= 0.0f)
		SetHidden(true);
}
