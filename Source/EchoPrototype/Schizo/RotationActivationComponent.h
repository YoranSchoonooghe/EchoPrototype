#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActivatableInterface.h"
#include "RotationActivationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API URotationActivationComponent : public UActorComponent, public IActivatableInterface
{
	GENERATED_BODY()

public:	
	URotationActivationComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FRotator TargetRotation{ FRotator(0, 90, 0) };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float Duration{ 1.0f };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate_Implementation() override;

private:
	FRotator _startRotation;
	bool _bIsRotating{ false };
	float _elapsedTime{ 0.0f };
};
