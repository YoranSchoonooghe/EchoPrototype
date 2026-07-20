#pragma once

#include "CoreMinimal.h"
#include "SchizoEffect.h"
#include "SE_CollapsingLedge.generated.h"

UCLASS()
class ECHOPROTOTYPE_API USE_CollapsingLedge : public USchizoEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collapsing Ledge")
	bool bHideActor{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collapsing Ledge")
	bool bDisableCollision{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collapsing Ledge")
	float CollapseDelay{ 1.0f };

	virtual void Execute(AActor* OwningActor) override;
	virtual void Update(AActor* OwningActor, float DeltaTime) override;

private:
	void Collapse();

	bool _bIsArmed{ false };
	bool _bIsCollapsing{ false };
	FTimerHandle CollapseTimerHandle;
	TWeakObjectPtr<AActor> CachedOwningActor;
};
