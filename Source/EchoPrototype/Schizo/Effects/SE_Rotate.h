#pragma once

#include "CoreMinimal.h"
#include "SchizoEffect.h"
#include "SE_Rotate.generated.h"

UCLASS()
class ECHOPROTOTYPE_API USE_Rotate : public USchizoEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bRotateInfinitely{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "!bRotateInfinitely"))
	FRotator TargetRotation{ FRotator(0, 90, 0) };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "!bRotateInfinitely"))
	float Duration{ 1.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "!bRotateInfinitely"))
	float Delay{ 0.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "!bRotateInfinitely"))
	bool bRepeat{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "!bRotateInfinitely && bRepeat"))
	float RestTime{ 0.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "bRotateInfinitely"))
	float AngularSpeed{ 90.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (EditCondition = "bRotateInfinitely"))
	FVector RotationAxis{ FVector::UpVector };

	virtual void Initialize(AActor* OwningActor) override;
	virtual void Update(AActor* OwningActor, float DeltaTime) override;
	virtual void Execute(AActor* OwningActor) override;

private:
	bool FinishedDelay();

	FRotator _startRotation;
	FRotator _targetRotation;
	bool _bIsRotating{ false };
	float _elapsedTime{ 0.0f };
	bool _bFinishedDelay{ false };
};
