#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "SkillTreeConnectorWidget.generated.h"

class SSkillTreeConnectors;

USTRUCT(BlueprintType)
struct FSkillTreeConnector
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	FVector2D Start = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	FVector2D End = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	FLinearColor Color = FLinearColor::Gray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	float Thickness = 3.0f;
};

UCLASS()
class ECHOPROTOTYPE_API USkillTreeConnectorWidget : public UWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void SetConnectors(const TArray<FSkillTreeConnector>& InConnectors);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TSharedPtr<SSkillTreeConnectors> MySlateWidget;
};
