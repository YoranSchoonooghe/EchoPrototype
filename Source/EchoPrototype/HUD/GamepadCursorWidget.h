#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamepadCursorWidget.generated.h"

class FAnalogCursor;
class FGamepadKeyButtonInputProcessor;
class UButton;


USTRUCT(BlueprintType)
struct FGamepadKeyButtonBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamepad Cursor")
	FKey Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gamepad Cursor")
	FName ButtonName;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Gamepad Cursor")
	TObjectPtr<UButton> Button;
};

UCLASS(Abstract)
class ECHOPROTOTYPE_API UGamepadCursorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Gamepad Cursor")
	float CursorAcceleration = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gamepad Cursor")
	float CursorMaxSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gamepad Cursor", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CursorDeadZone = 0.15f;

	UFUNCTION(BlueprintCallable, Category = "Gamepad Cursor")
	void RegisterKeyButtonBinding(FKey Key, UButton* Button);

	UFUNCTION(BlueprintCallable, Category = "Gamepad Cursor")
	void ClearKeyButtonBindings();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gamepad Cursor")
	TArray<FGamepadKeyButtonBinding> KeyButtonBindings;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void HandleBoundKeyPressed(FKey Key);

	TSharedPtr<FAnalogCursor> AnalogCursor;
	TSharedPtr<FGamepadKeyButtonInputProcessor> KeyButtonInputProcessor;
};
