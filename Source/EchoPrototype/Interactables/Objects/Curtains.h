
#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "Curtains.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class ECurtainsState : uint8
{
	Closed,
	Opening,
	Open,
	Closing
};

UCLASS()
class ECHOPROTOTYPE_API ACurtains : public AInteractableActor
{
	GENERATED_BODY()

public:
	ACurtains();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "Curtains")
	FORCEINLINE ECurtainsState GetCurtainsState() const { return CurtainsState; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Curtains", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> CurtainsRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Curtains", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeftCurtainMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Curtains", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RightCurtainMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Curtains|Kill", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> KillZone;

	UFUNCTION(BlueprintCallable, Category = "Curtains")
	void OpenCurtains();

	UFUNCTION(BlueprintCallable, Category = "Curtains")
	void CloseCurtains();

	UFUNCTION(BlueprintCallable, Category = "Curtains")
	void ToggleCurtains();

	UPROPERTY(EditAnywhere, Category = "Curtains")
	float OpenDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Curtains")
	float OpenSpeed = 400.0f;

private:
	void KillEnemiesInFront();

	ECurtainsState CurtainsState = ECurtainsState::Closed;

	FVector LeftClosedLocation = FVector::ZeroVector;
	FVector LeftOpenLocation = FVector::ZeroVector;
	FVector RightClosedLocation = FVector::ZeroVector;
	FVector RightOpenLocation = FVector::ZeroVector;
};
