#include "EchoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "EchoCombatComponent.h"

AEchoAIController::AEchoAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEchoAIController::BeginPlay()
{
	Super::BeginPlay();

	//RunBehaviorTree(BTEcho);

	//AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEchoAIController::HandlePerception);
}

void AEchoAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;
	
	auto* pEnemy = Cast<AEnemyCharacter>(Actor);
	if (!pEnemy) return;

	pBlackboardComponent->SetValueAsObject(TEXT("TargetEnemy"), pEnemy);
}
