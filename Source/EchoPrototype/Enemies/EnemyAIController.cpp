#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "EchoPrototype/Character/PlayerCharacter.h"
#include "EchoPrototype/Combat/HealthComponent.h"
#include "EchoPrototype/Echo/EchoActor.h"

AEnemyAIController::AEnemyAIController()
{
	//PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BTEnemy);
	InitBBKeys();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandlePerception);

	auto* pHealth = GetPawn()->GetComponentByClass<UHealthComponent>();
	if (pHealth)
	{
		pHealth->OnDamage.AddDynamic(this, &AEnemyAIController::SetTargetActor);
	}
}

void AEnemyAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto const TargetPlayerKeyName = TEXT("TargetPlayer");

	auto* pPlayer = Cast<APlayerCharacter>(Actor);
	if (pPlayer)
	{
		auto* pBlackboardComponent = GetBlackboardComponent();
		if (!pBlackboardComponent) return;

		if (Stimulus.WasSuccessfullySensed())
		{
			pBlackboardComponent->SetValueAsObject(TargetPlayerKeyName, pPlayer);
		}
		else
		{
			pBlackboardComponent->ClearValue(TargetPlayerKeyName);
			pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), pPlayer->GetActorLocation());
		}

		return;
	}

	auto* pEcho = Cast<AEchoActor>(Actor);
	if (pEcho)
	{
		if (pEcho->GetVisualState() == EEchoVisualState::Placed)
		{
			auto* pBlackboardComponent = GetBlackboardComponent();
			if (!pBlackboardComponent) return;

			pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), pEcho->GetActorLocation());
		}
		else
		{
			if (_targetEcho)
				_targetEcho->OnPlaced.RemoveDynamic(this, &AEnemyAIController::UpdateTargetEcho);

			_targetEcho = pEcho;
			_targetEcho->OnPlaced.AddDynamic(this, &AEnemyAIController::UpdateTargetEcho);
		}

	}
}

void AEnemyAIController::SetTargetActor(AActor* Aggressor)
{
	auto const TargetPlayerKeyName = TEXT("TargetPlayer");

	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	auto* pAggressorCharacter = Cast<APlayerCharacter>(Aggressor);
	if (!pAggressorCharacter) return;

	if (!pBlackboardComponent->GetValueAsObject(TargetPlayerKeyName))
	{
		pBlackboardComponent->SetValueAsObject(TargetPlayerKeyName, pAggressorCharacter);
	}
}

void AEnemyAIController::InitBBKeys()
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	auto* pEnemy = Cast<AEnemyCharacter>(GetCharacter());
	if (!pEnemy) return;

	if (pEnemy->bIsGuarding)
	{
		pBlackboardComponent->SetValueAsVector(TEXT("GuardLocation"), GetPawn()->GetActorLocation());
		pBlackboardComponent->SetValueAsRotator(TEXT("GuardRotation"), GetPawn()->GetActorRotation());

		return;
	}

	auto patrolPoints = pEnemy->PatrolPoints;
	if (patrolPoints.IsEmpty()) return;

	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPointIndex"), 0);
	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPoint"), patrolPoints[0]);
}

void AEnemyAIController::UpdateTargetEcho()
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	pBlackboardComponent->SetValueAsVector(TEXT("SusLocation"), _targetEcho->GetActorLocation());
}
