#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolRoute.generated.h"

class USplineComponent;
class UEchoVisibilityComponent;
class APatrolPoint;

UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	Single,
	Loop,
	Reverse
};

UCLASS()
class ECHOPROTOTYPE_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolRoute();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USplineComponent> PatrolSpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UEchoVisibilityComponent> EchoVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<TObjectPtr<APatrolPoint>> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	EPatrolMode PatrolMode{ EPatrolMode::Loop };

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	TArray<APatrolPoint*> GetPatrolPoints() const { return PatrolPoints; }
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	int GetNextPatrolPointIndex(int currentPointIndex);
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	APatrolPoint* GetPatrolPointAtIndex(int index) const;

	UFUNCTION(CallInEditor, Category = "Patrol")
	void RebuildPatrolSpline();

private:
	int GetNextPatrolPointIndexSingle(int currentPointIndex);
	int GetNextPatrolPointIndexLoop(int currentPointIndex);
	int GetNextPatrolPointIndexReverse(int currentPointIndex);

	bool _bIsReversed{ false };

};
