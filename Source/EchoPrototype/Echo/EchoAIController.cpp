#include "EchoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "EchoCombatComponent.h"
#include "EchoCharacter.h"

AEchoAIController::AEchoAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEchoAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEchoAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	_controlledCharacter = Cast<AEchoCharacter>(InPawn);
	if (!_controlledCharacter) return;

	_controlledCharacter->OnPlaced.AddDynamic(this, &AEchoAIController::InitializeBehaviorTree);
}

void AEchoAIController::HandlePerception(AActor* Actor, FAIStimulus Stimulus)
{
	auto* pBlackboardComponent = GetBlackboardComponent();
	if (!pBlackboardComponent) return;
	
	auto* pEnemy = Cast<AEnemyCharacter>(Actor);
	if (!pEnemy) return;

	pBlackboardComponent->SetValueAsObject(TEXT("TargetEnemy"), pEnemy);
}

void AEchoAIController::InitializeBehaviorTree()
{
	if (_controlledCharacter->GetActiveEchoType() != EEchoType::Combat) return;
	
	RunBehaviorTree(BTEcho);

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEchoAIController::HandlePerception);
}
