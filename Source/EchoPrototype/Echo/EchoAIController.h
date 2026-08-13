#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EchoAIController.generated.h"

class AEchoCharacter;

UCLASS()
class ECHOPROTOTYPE_API AEchoAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEchoAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void HandlePerception(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BTEcho;

private:
	UFUNCTION()
	void InitializeBehaviorTree();

	AEchoCharacter* _controlledCharacter{ nullptr };

};
