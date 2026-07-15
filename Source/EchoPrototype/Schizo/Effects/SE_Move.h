#pragma once

#include "CoreMinimal.h"
#include "SchizoEffect.h"
#include "SE_Move.generated.h"

UCLASS()
class ECHOPROTOTYPE_API USE_Move : public USchizoEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetMovement{ FVector(100, 0, 0) };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Duration{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bLoop{ false };

	virtual void Update(AActor* OwningActor, float DeltaTime) override;
	virtual void Execute(AActor* OwningActor) override;

private:
	FVector _startLocation;
	FVector _targetLocation;
	bool _bIsMoving{ false };
	float _elapsedTime{ 0.0f };

};
