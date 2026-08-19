#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolRoute.generated.h"

class USplineComponent;
class UEchoVisibilityComponent;

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

public:	
	virtual void Tick(float DeltaTime) override;

};
