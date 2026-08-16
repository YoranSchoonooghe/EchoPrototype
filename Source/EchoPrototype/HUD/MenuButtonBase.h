#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Containers/Ticker.h"
#include "Styling/SlateTypes.h"
#include "MenuButtonBase.generated.h"

UCLASS()
class ECHOPROTOTYPE_API UMenuButtonBase : public UButton
{
	GENERATED_BODY()

public:
	UMenuButtonBase(const FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Button|Bump")
	float BumpScale = 1.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Button|Bump")
	float BumpAngle = -3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Button|Bump")
	float BumpInterpSpeed = 12.f;

private:
	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	void HandleReceivedFocus();
	void HandleLostFocus();

	void RefreshBumpState();
	bool TickAnimation(float DeltaTime);

	void RefreshVisualStyle();

	bool bIsMouseHovered = false;
	bool bIsFocused = false;

	FVector2D TargetScale = FVector2D::UnitVector;
	float TargetAngle = 0.f;

	FSlateBrush SavedNormalBrush;
	bool bHasSavedNormalBrush = false;

	FTSTicker::FDelegateHandle AnimationTickerHandle;
};
