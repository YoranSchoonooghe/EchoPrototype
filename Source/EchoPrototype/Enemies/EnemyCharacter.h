#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolPoint.h"
#include "EnemyCharacter.generated.h"

class UHealthComponent;
class UCombatComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EAlertState : uint8
{
	Neutral,
	Suspicious,
	Alert
};

UCLASS()
class ECHOPROTOTYPE_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> AlertWidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Alert")
	EAlertState AlertState{ EAlertState::Neutral };

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Alert")
	void ChangeAlertState(EAlertState state);

	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	TArray<APatrolPoint*> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAI")
	bool bIsGuarding{ false };
};
