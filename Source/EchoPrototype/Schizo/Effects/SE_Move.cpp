#include "SE_Move.h"

void USE_Move::Update(AActor* OwningActor, float DeltaTime)
{
	if (!_bIsMoving) return;

	_elapsedTime += DeltaTime;

	float alpha = FMath::Clamp(_elapsedTime / Duration, 0.0f, 1.0f);
	alpha = FMath::InterpEaseInOut(0.f, 1.f, alpha, 2.0f);
	FVector location = FMath::Lerp(_startLocation, _targetLocation, alpha);

	OwningActor->SetActorLocation(location);

	if (alpha >= 1.0f)
	{
		if (bLoop)
		{
			FVector const newTargetLocation = _startLocation;
			_startLocation = _targetLocation;
			_targetLocation = newTargetLocation;

			_elapsedTime = 0.0f;
		}
		else
		{
			_bIsMoving = false;
		}
	}
}

void USE_Move::Execute(AActor* OwningActor)
{
	_startLocation = OwningActor->GetActorLocation();
	_targetLocation = _startLocation + TargetMovement;

	_elapsedTime = 0.0f;
	_bIsMoving = true;
}
