// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chandelier.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ALever;
class ACuttableRope;

UENUM(BlueprintType)
enum class EChandelierState : uint8
{
	Hanging,
	Falling,
	Landed
};

UCLASS()
class ECHOPROTOTYPE_API AChandelier : public AActor
{
	GENERATED_BODY()

public:
	AChandelier();

	UFUNCTION(BlueprintCallable, Category = "Chandelier")
	void TriggerDrop();

	UFUNCTION(BlueprintPure, Category = "Chandelier")
	FORCEINLINE EChandelierState GetChandelierState() const { return ChandelierState; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chandelier", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ChandelierMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chandelier|Kill", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> KillZone;

	UPROPERTY(EditAnywhere, Category = "Chandelier|Triggers")
	TObjectPtr<ALever> TriggerLever;

	UPROPERTY(EditAnywhere, Category = "Chandelier|Triggers")
	TObjectPtr<ACuttableRope> TriggerRope;

	UFUNCTION(BlueprintImplementableEvent, Category = "Chandelier")
	void OnChandelierLanded();

private:
	UFUNCTION()
	void OnChandelierMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnKillZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EChandelierState ChandelierState = EChandelierState::Hanging;
};
