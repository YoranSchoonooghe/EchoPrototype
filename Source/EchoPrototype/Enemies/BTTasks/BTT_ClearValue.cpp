#include "BTT_ClearValue.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_ClearValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pBlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!pBlackboardComponent)
        return EBTNodeResult::Failed;

    pBlackboardComponent->ClearValue(KeyToClear.SelectedKeyName);

    return EBTNodeResult::Succeeded;
}
