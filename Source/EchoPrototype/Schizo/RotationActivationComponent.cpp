#include "RotationActivationComponent.h"

URotationActivationComponent::URotationActivationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void URotationActivationComponent::BeginPlay()
{
	Super::BeginPlay();

	_startRotation = GetOwner()->GetActorRotation();
}


void URotationActivationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!_bIsRotating) return;

	_elapsedTime += DeltaTime;

	float alpha = FMath::Clamp(_elapsedTime / Duration, 0.0f, 1.0f);
	FRotator rotation = FMath::Lerp(_startRotation, TargetRotation, alpha);

	GetOwner()->SetActorRotation(rotation);

	if (alpha >= 1.0f)
	{
		_bIsRotating = false;
	}
}

void URotationActivationComponent::Activate_Implementation()
{
	_startRotation = GetOwner()->GetActorRotation();
	_elapsedTime = 0.0f;
	_bIsRotating = true;
}

