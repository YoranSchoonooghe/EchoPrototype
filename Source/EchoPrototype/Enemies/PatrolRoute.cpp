#include "PatrolRoute.h"
#include "Components/SplineComponent.h"
#include "EchoPrototype/Echo/EchoVisibilityComponent.h"

APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = true;

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

