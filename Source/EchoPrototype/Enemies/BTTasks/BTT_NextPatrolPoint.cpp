#include "BTT_NextPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"

EBTNodeResult::Type UBTT_NextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pBlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!pBlackboardComponent)
        return EBTNodeResult::Failed;

    auto* pAIController = OwnerComp.GetAIOwner();
    if (!pAIController)
        return EBTNodeResult::Failed;

    auto* pEnemyCharacter = Cast<AEnemyCharacter>(pAIController->GetCharacter());
    if (!pEnemyCharacter)
        return EBTNodeResult::Failed;

    auto patrolPointIndex = pBlackboardComponent->GetValueAsInt(PatrolPointIndex.SelectedKeyName);

    ++patrolPointIndex;
    patrolPointIndex %= pEnemyCharacter->PatrolPoints.Num();

    pBlackboardComponent->SetValueAsObject(PatrolPoint.SelectedKeyName, pEnemyCharacter->PatrolPoints[patrolPointIndex]);
    pBlackboardComponent->SetValueAsInt(PatrolPointIndex.SelectedKeyName, patrolPointIndex);

    return EBTNodeResult::Succeeded;
}
