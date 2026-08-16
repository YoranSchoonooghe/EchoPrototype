#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionMeterWidget.generated.h"

class AEnemyCharacter;
class AEnemyAIController;

UCLASS()
class ECHOPROTOTYPE_API UDetectionMeterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetEnemy(AEnemyCharacter* Enemy);
	void SetHidden(bool hidden = true);

protected:
	UFUNCTION(BlueprintCallable, Category = "Detection")
	float GetDetectionValue() const;

private:
	UFUNCTION()
	void UpdateVisibility(float DetectionValue);

	AEnemyAIController* _enemyController{ nullptr };
};
