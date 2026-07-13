#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class AEchoActor;

UCLASS()
class ECHOPROTOTYPE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BTEnemy;

	UFUNCTION()
	void HandlePerception(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void SetTargetActor(AActor* Aggressor);

private:
	void InitBBKeys();

	UFUNCTION()
	void UpdateTargetEcho();

	void HandleSightPerception(AActor* Actor, FAIStimulus Stimulus);
	void HandleSoundPerception(AActor* Actor, FAIStimulus Stimulus);

	AEchoActor* _targetEcho;
	TArray<AActor*> _spottedEchoes;
};
