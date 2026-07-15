#include "SpinningBlade.h"
#include "EchoPrototype/Schizo/SchizoComponent.h"

ASpinningBlade::ASpinningBlade()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SchizoComponent = CreateDefaultSubobject<USchizoComponent>(TEXT("SchizoComponent"));
}

void ASpinningBlade::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpinningBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

