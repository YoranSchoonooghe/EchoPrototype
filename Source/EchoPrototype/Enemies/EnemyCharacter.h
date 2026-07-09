#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolPoint.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ECHOPROTOTYPE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	TArray<APatrolPoint*> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	bool IsStationary{ false };
};
