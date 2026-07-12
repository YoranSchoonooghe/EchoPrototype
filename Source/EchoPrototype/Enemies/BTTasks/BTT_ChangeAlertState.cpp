#include "BTT_ChangeAlertState.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_ChangeAlertState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pAIController = OwnerComp.GetAIOwner();
    if (!pAIController)
        return EBTNodeResult::Failed;

    auto* pEnemy = Cast<AEnemyCharacter>(pAIController->GetCharacter());
    if (!pEnemy)
        return EBTNodeResult::Failed;

    pEnemy->ChangeAlertState(alertState);

    return EBTNodeResult::Succeeded;
}