#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "SchizoEnemyCharacter.generated.h"

class USchizoComponent;

UCLASS()
class ECHOPROTOTYPE_API ASchizoEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
public:
	ASchizoEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Schizo")
	TObjectPtr<USchizoComponent> SchizoComponent;

public:
	virtual void Tick(float DeltaTime) override;

};
