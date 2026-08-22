#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class AEchoCharacter;
class APlayerCharacter;
class AEnemyCharacter;

UCLASS()
class ECHOPROTOTYPE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	UFUNCTION(BlueprintCallable, Category = "Perception|Detection")
	float GetDetectionValue() const { return _detectionValue; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetectionValueChangedSignature, float, DetectionValue);
	UPROPERTY(BlueprintAssignable, Category = "Perception|Detection")
	FOnDetectionValueChangedSignature OnDetectionValueChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BTEnemy;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Reachability")
	bool bIgnoreUnreachableTargets = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Reachability", meta = (EditCondition = "bIgnoreUnreachableTargets"))
	float MaxReachableHeightAboveSelf = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Reachability", meta = (EditCondition = "bIgnoreUnreachableTargets"))
	bool bRequireNavPath = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Reachability", meta = (EditCondition = "bIgnoreUnreachableTargets"))
	float ReachabilityCheckInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Echo Attack")
	float EchoAttackRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Echo Attack")
	float EchoAttackCooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Echo Attack")
	float EchoAttackCheckInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Memory")
	float ForgetTargetDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Detection")
	float DetectionThreshold{ 1.0f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Detection")
	float DetectionRate{ 1.0f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception|Detection")
	float DecayRate{ 0.7f };

	UFUNCTION()
	void HandlePerception(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void SetTargetActor(AActor* Aggressor);
	UFUNCTION()
	void HandleStunStateChanged(bool bIsStunned);

private:
	void InitBBKeys();

	UFUNCTION()
	void UpdateTargetEcho();

	void HandleSightPerception(AActor* Actor, FAIStimulus Stimulus);
	void HandleSoundPerception(AActor* Actor, FAIStimulus Stimulus);

	void UpdateDetection(float DeltaTime);
	void SpotPlayer();

	bool IsEchoDetectable(AEchoCharacter* Echo) const;

	bool IsLocationReachable(const FVector& Location) const;

	void StartReachabilityMonitor();
	void StopReachabilityMonitor();
	void CheckTargetReachability();

	void ForgetTarget();

	void HandleDetectableEchoSpotted(AEchoCharacter* Echo);

	void StartEchoAttackMonitor(AEchoCharacter* Echo);
	void StopEchoAttackMonitor();
	void CheckEchoAttackRange();
	void AttackTargetEcho();

	AEchoCharacter* _targetEcho;
	TArray<AActor*> _spottedEchoes;

	AEnemyCharacter* _controlledEnemy;
	bool _bIsPlayerInSight{ false };
	APlayerCharacter* _targetPlayer{ nullptr };
	float _detectionValue{ 0.0f };

	FTimerHandle ReachabilityCheckTimerHandle;
	FTimerHandle ForgetTargetTimerHandle;
	FTimerHandle EchoAttackCheckTimerHandle;
	float LastEchoAttackTime = -1000.0f;
};
