#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "AlertWidget.generated.h"

UCLASS()
class ECHOPROTOTYPE_API UAlertWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Alert")
	void UpdateAlertIcon(EAlertState alertState);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* SuspiciousTexture;
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* AlertTexture;
};
