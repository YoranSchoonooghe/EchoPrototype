// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindWanderSpot.generated.h"

/**
 * 
 */
UCLASS()
class ECHOPROTOTYPE_API UBTT_FindWanderSpot : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Wander")
	float WanderRadius{ 1000.0f };
};
