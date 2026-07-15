#include "RotatingStaircase.h"
#include "EchoPrototype/Schizo/SchizoComponent.h"

ARotatingStaircase::ARotatingStaircase()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SchizoComponent = CreateDefaultSubobject<USchizoComponent>(TEXT("SchizoComponent"));
}

void ARotatingStaircase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARotatingStaircase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

