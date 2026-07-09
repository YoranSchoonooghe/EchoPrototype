// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"

AEnemyAIController::AEnemyAIController()
{
	//PrimaryActorTick.bCanEverTick = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();


}
