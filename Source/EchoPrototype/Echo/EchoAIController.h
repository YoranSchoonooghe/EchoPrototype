#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EchoAIController.generated.h"

UCLASS()
class ECHOPROTOTYPE_API AEchoAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEchoAIController();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePerception(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BTEcho;

};
