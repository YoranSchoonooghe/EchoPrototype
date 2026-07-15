#include "SE_Rotate.h"

void USE_Rotate::Initialize(AActor* OwningActor)
{
	if (bRotateInfinitely)
	{
		RotationAxis = RotationAxis.GetSafeNormal();
	}
}

void USE_Rotate::Update(AActor* OwningActor, float DeltaTime)
{
	if (!_bIsRotating) return;

	_elapsedTime += DeltaTime;

	if (!FinishedDelay()) return;

	if (bRotateInfinitely)
	{
		auto rotation = OwningActor->GetActorRotation().Quaternion();
		
		FQuat deltaRotation = FQuat(RotationAxis, FMath::DegreesToRadians(AngularSpeed * DeltaTime));
		rotation *= deltaRotation;

		OwningActor->SetActorRotation(rotation);
	}
	else
	{
		float alpha = FMath::Clamp(_elapsedTime / Duration, 0.0f, 1.0f);
		alpha = FMath::InterpEaseInOut(0.f, 1.f, alpha, 2.0f);
		FRotator rotation = FMath::Lerp(_startRotation, _targetRotation, alpha);

		OwningActor->SetActorRotation(rotation);

		if (alpha >= 1.0f)
		{
			if (bRepeat)
			{
				if (_elapsedTime - Duration < RestTime) return;

				FRotator const newTargetRotation = _startRotation;
				_startRotation = _targetRotation;
				_targetRotation = newTargetRotation;

				_elapsedTime = 0.0f;
			}
			else
			{
				_bIsRotating = false;
			}
		}
	}
}

void USE_Rotate::Execute(AActor* OwningActor)
{
	_startRotation = OwningActor->GetActorRotation();
	_targetRotation = _startRotation + TargetRotation;

	_elapsedTime = 0.0f;
	_bIsRotating = true;
}

bool USE_Rotate::FinishedDelay()
{
	if (_bFinishedDelay)
		return true;

	if (_elapsedTime < Delay)
	{
		return false;
	}

	_elapsedTime -= Delay;
	_bFinishedDelay = true;
	return true;
}
