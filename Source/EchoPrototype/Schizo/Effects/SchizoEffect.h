#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SchizoEffect.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ECHOPROTOTYPE_API USchizoEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* OwningActor) {};
	virtual void Update(AActor* OwningActor, float DeltaTime) {};

	virtual void Execute(AActor* OwningActor) PURE_VIRTUAL(USchizoEffect::Execute, );
};
