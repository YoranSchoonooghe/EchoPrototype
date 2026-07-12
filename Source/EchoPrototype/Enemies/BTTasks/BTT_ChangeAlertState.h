#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EchoPrototype/Enemies/EnemyCharacter.h"
#include "BTT_ChangeAlertState.generated.h"

UCLASS()
class ECHOPROTOTYPE_API UBTT_ChangeAlertState : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Alert")
	EAlertState alertState{ EAlertState::Neutral };
};
