// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindWanderSpot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

EBTNodeResult::Type UBTT_FindWanderSpot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* pBlackboardComponent = OwnerComp.GetBlackboardComponent();

    if (!pBlackboardComponent)
        return EBTNodeResult::Failed;

    auto* pAIController = OwnerComp.GetAIOwner();
    if (!pAIController)
        return EBTNodeResult::Failed;

    APawn* pPawn = pAIController->GetPawn();
    if (!pPawn)
        return EBTNodeResult::Failed;

    UNavigationSystemV1* pNavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!pNavSystem)
        return EBTNodeResult::Failed;

    FNavLocation WanderLocation;

    bool success = pNavSystem->GetRandomReachablePointInRadius(
        pPawn->GetActorLocation(),
        WanderRadius,
        WanderLocation
    );

    if (success)
    {
        pBlackboardComponent->SetValueAsVector(TargetLocation.SelectedKeyName, WanderLocation.Location);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}