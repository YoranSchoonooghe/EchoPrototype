#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActivatableInterface.h"
#include "VisibilityActivationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API UVisibilityActivationComponent : public UActorComponent, public IActivatableInterface
{
	GENERATED_BODY()

public:	
	UVisibilityActivationComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
	bool bStartHidden{ true };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
	bool bUseCollision{ true };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate_Implementation() override;
};
