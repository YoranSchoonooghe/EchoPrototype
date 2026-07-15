#include "SchizoActor.h"
#include "SchizoComponent.h"
#include "VisibilityActivationComponent.h"
#include "RotationActivationComponent.h"

ASchizoActor::ASchizoActor()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	VisibilityComponent = CreateDefaultSubobject<UVisibilityActivationComponent>(TEXT("VisibilityActivationComponent"));
	RotationComponent = CreateDefaultSubobject<URotationActivationComponent>(TEXT("RotationActivationComponent"));

	SchizoComponent = CreateDefaultSubobject<USchizoComponent>(TEXT("SchizoComponent"));
}

void ASchizoActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASchizoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

