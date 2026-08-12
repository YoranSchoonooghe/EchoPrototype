#pragma once

#include "CoreMinimal.h"
#include "EchoComponentBase.h"
#include "EchoDistractionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ECHOPROTOTYPE_API UEchoDistractionComponent : public UEchoComponentBase
{
	GENERATED_BODY()

public:
	UEchoDistractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo|Distraction")
	float SignalInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo|Distraction")
	float SignalLoudness = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo|Distraction")
	float SignalMaxRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo|Distraction")
	FName SignalTag = "Distraction";

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void HandlePlaced();

	UFUNCTION()
	void HandleDied();

	void EmitDistractionSignal();
	void RemoveDistraction();

	FTimerHandle SignalTimerHandle;
};
