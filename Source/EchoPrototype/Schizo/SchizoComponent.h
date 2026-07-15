#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Effects/SchizoEffect.h"
#include "SchizoComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API USchizoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USchizoComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schizo")
	int EchoCountTrigger{ 1 };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Instanced, Category = "Schizo")
	TArray<TObjectPtr<USchizoEffect>> SchizoEffects;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivatedSignature);
	UPROPERTY(BlueprintAssignable, Category = "Schizo")
	FOnActivatedSignature OnActivated;

private:
	void BindToPlayerEchoPlaced();

	UFUNCTION()
	void IncrementEchoCount();
	
	void ActivateSchizoEffect();

	int _echoCount{ 0 };
};
