#include "PatrolPoint.h"

APatrolPoint::APatrolPoint()
{
	//PrimaryActorTick.bCanEverTick = true;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(RootComponent);

	PreviewMesh->SetHiddenInGame(true);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

//void APatrolPoint::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

