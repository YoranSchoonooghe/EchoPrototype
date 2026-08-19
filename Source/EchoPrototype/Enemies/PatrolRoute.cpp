#include "PatrolRoute.h"
#include "Components/SplineComponent.h"
#include "EchoPrototype/Echo/EchoVisibilityComponent.h"

APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	PatrolSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolSpline"));
	PatrolSpline->SetupAttachment(pRoot);

	EchoVisibility = CreateDefaultSubobject<UEchoVisibilityComponent>(TEXT("EchoVisibility"));
}

void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatrolRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int APatrolRoute::GetNextPatrolPointIndex(int currentPointIndex)
{
	if (PatrolPoints.IsEmpty()) return -1;
	if (PatrolPoints.Num() == 1) return 0;

	switch (PatrolMode)
	{
	case EPatrolMode::Single:
		return GetNextPatrolPointIndexSingle(currentPointIndex);
		break;
	case EPatrolMode::Loop:
		return GetNextPatrolPointIndexLoop(currentPointIndex);
		break;
	case EPatrolMode::Reverse:
		return GetNextPatrolPointIndexReverse(currentPointIndex);
		break;
	}
	
	return -1;
}

APatrolPoint* APatrolRoute::GetPatrolPointAtIndex(int index) const
{
	if (index < 0 || index >= PatrolPoints.Num()) return nullptr;

	return PatrolPoints[index];
}

int APatrolRoute::GetNextPatrolPointIndexSingle(int currentPointIndex)
{
	if (currentPointIndex == PatrolPoints.Num() - 1)
	{
		return currentPointIndex;
	}

	return (currentPointIndex + 1);
}

int APatrolRoute::GetNextPatrolPointIndexLoop(int currentPointIndex)
{
	int nextIndex = currentPointIndex + 1;
	nextIndex %= PatrolPoints.Num();

	return nextIndex;
}

int APatrolRoute::GetNextPatrolPointIndexReverse(int currentPointIndex)
{
	if (_bIsReversed)
	{
		if (currentPointIndex == 0)
		{
			_bIsReversed = false;

			return (currentPointIndex + 1);
		}

		return (currentPointIndex - 1);
	}
	else
	{
		if (currentPointIndex == PatrolPoints.Num() - 1)
		{
			_bIsReversed = true;

			return (currentPointIndex - 1);
		}

		return (currentPointIndex + 1);
	}
}

