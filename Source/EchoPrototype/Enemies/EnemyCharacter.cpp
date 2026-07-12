#include "EnemyCharacter.h"
#include "../Combat/HealthComponent.h"
#include "EchoPrototype/Combat/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "BPWidgets/AlertWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	AlertWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	AlertWidgetComp->SetupAttachment(GetMesh());
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::ChangeAlertState(EAlertState state)
{
	AlertState = state;

	if (UAlertWidget* Widget = Cast<UAlertWidget>(AlertWidgetComp->GetUserWidgetObject()))
	{
		Widget->UpdateAlertIcon(state);
	}
}

