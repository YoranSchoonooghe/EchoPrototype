#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API UEchoCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEchoCombatComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed{ 600.0f };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
