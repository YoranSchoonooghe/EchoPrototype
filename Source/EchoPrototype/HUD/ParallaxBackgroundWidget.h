#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParallaxBackgroundWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTexture2D;

USTRUCT(BlueprintType)
struct FParallaxLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parallax")
	TObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parallax")
	float ParallaxStrength = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parallax", meta = (ClampMin = "1.0"))
	float OversizeScale = 1.15f;
};

UCLASS()
class ECHOPROTOTYPE_API UParallaxBackgroundWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetGamepadStickInput(const FVector2D& StickValue);

protected:
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> LayerContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Parallax")
	TArray<FParallaxLayer> Layers;

	UPROPERTY(EditDefaultsOnly, Category = "Parallax")
	float MaxOffsetPixels = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Parallax")
	float InterpSpeed = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Parallax", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StickDeadzone = 0.15f;

private:
	void BuildLayers();

	UPROPERTY()
	TArray<TObjectPtr<UImage>> LayerImages;

	FVector2D MouseTargetOffset = FVector2D::ZeroVector;
	FVector2D StickTargetOffset = FVector2D::ZeroVector;
	bool bStickActive = false;

	FVector2D CurrentOffset = FVector2D::ZeroVector;
};
