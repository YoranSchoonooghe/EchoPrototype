#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SchizoActor.generated.h"

class USchizoComponent;
class UVisibilityActivationComponent;
class URotationActivationComponent;

UCLASS()
class ECHOPROTOTYPE_API ASchizoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASchizoActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USchizoComponent> SchizoComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UVisibilityActivationComponent> VisibilityComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<URotationActivationComponent> RotationComponent;

public:
	virtual void Tick(float DeltaTime) override;

};
