#include "BTT_MeleeAttack.h"
#include "AIController.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "EchoPrototype/Combat/CombatComponent.h"

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pAIController = OwnerComp.GetAIOwner();
    if (!pAIController)
        return EBTNodeResult::Failed;

    auto pCombatComponent = pAIController->GetCharacter()->FindComponentByClass<UCombatComponent>();
    if (!pCombatComponent)
        return EBTNodeResult::Failed;

    pCombatComponent->OnAttackReleased();
    return EBTNodeResult::Succeeded;
}