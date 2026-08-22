#include "EnemyCharacter.h"
#include "../Combat/HealthComponent.h"
#include "EchoPrototype/Combat/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "BPWidgets/AlertWidget.h"
#include "BPWidgets/DetectionMeterWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	AlertWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("AlertWidgetComponent"));
	AlertWidgetComp->SetupAttachment(GetMesh());
	DetectionWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionWidgetComponent"));
	DetectionWidgetComp->SetupAttachment(GetMesh());

	StealthKillPromptWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("StealthKillPromptWidgetComponent"));
	StealthKillPromptWidgetComp->SetupAttachment(GetMesh());
	StealthKillPromptWidgetComp->SetVisibility(false);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto* DetectionWidget = Cast<UDetectionMeterWidget>(DetectionWidgetComp->GetUserWidgetObject()))
	{
		DetectionWidget->SetEnemy(this);
		DetectionWidget->SetHidden(true);
	}

	if (bStartsArmed && Combat)
	{
		Combat->SetWeaponEquipped(true);
	}
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

	if (UAlertWidget* AlertWidget = Cast<UAlertWidget>(AlertWidgetComp->GetUserWidgetObject()))
	{
		AlertWidget->UpdateAlertIcon(state);

		if (auto* DetectionWidget = Cast<UDetectionMeterWidget>(DetectionWidgetComp->GetUserWidgetObject()))
		{
			if (state != EAlertState::Neutral)
				DetectionWidget->SetHidden(true);
		}
	}
}

void AEnemyCharacter::ShowStealthKillPrompt(bool bShow)
{
	if (StealthKillPromptWidgetComp)
	{
		StealthKillPromptWidgetComp->SetVisibility(bShow);
	}
}

