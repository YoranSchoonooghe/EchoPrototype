#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingStaircase.generated.h"

class USchizoComponent;

UCLASS()
class ECHOPROTOTYPE_API ARotatingStaircase : public AActor
{
	GENERATED_BODY()
	
public:	
	ARotatingStaircase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<USchizoComponent> SchizoComponent;

public:	
	virtual void Tick(float DeltaTime) override;

};
