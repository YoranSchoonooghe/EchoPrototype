#pragma once

#include "CoreMinimal.h"
#include "SchizoEffect.h"
#include "SE_ToggleVisibility.generated.h"

UCLASS()
class ECHOPROTOTYPE_API USE_ToggleVisibility : public USchizoEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	bool bStartHidden{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	bool bUseCollision{ false };

	virtual void Initialize(AActor* OwningActor) override;
	virtual void Execute(AActor* OwningActor) override;

private:
	void UpdateVisibility(AActor* OwningActor);

	bool _bIsVisible{ true };
};
