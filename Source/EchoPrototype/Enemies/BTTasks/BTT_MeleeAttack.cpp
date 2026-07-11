#include "BTT_MeleeAttack.h"
#include "AIController.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "EchoPrototype/Combat/CombatComponent.h"

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pAIController = OwnerComp.GetAIOwner();
    if (!pAIController)
        return EBTNodeResult::Failed;

    auto* pCharacter = Cast<AEnemyCharacter>(pAIController->GetCharacter());
    if (!pCharacter)
        return EBTNodeResult::Failed;

    if (UCombatComponent* pCombatComponent = pCharacter->GetCombatComponent())
    {
        pCombatComponent->TryAttack();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}