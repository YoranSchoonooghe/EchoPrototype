#pragma once

#include "CoreMinimal.h"
#include "EchoComponentBase.h"
#include "EchoDistractionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ECHOPROTOTYPE_API UEchoDistractionComponent : public UEchoComponentBase
{
	GENERATED_BODY()

public:	
	UEchoDistractionComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
