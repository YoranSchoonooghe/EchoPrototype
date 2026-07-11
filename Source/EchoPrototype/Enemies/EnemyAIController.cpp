#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "EchoPrototype/Character/PlayerCharacter.h"

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
}

void AEnemyAIController::InitBBKeys()
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;

	auto* pEnemy = Cast<AEnemyCharacter>(GetCharacter());
	if (!pEnemy) return;

	if (pEnemy->IsStationary)
	{
		pBlackboardComponent->SetValueAsBool(TEXT("IsStationary"), true);

		return;
	}

	auto patrolPoints = pEnemy->PatrolPoints;
	if (patrolPoints.IsEmpty()) return;

	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPointIndex"), 0);
	pBlackboardComponent->SetValueAsObject(TEXT("PatrolPoint"), patrolPoints[0]);
}

void AEnemyAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto* pPlayer = Cast<APlayerCharacter>(Actor);
	if (pPlayer)
	{
		auto* pBlackboardComponent = GetBlackboardComponent();
		if (!pBlackboardComponent) return;

		if (Stimulus.WasSuccessfullySensed())
		{
			pBlackboardComponent->SetValueAsObject(TEXT("TargetPlayer"), pPlayer);
		}
		else
		{
			pBlackboardComponent->ClearValue(TEXT("TargetPlayer"));
		}
	}
}
