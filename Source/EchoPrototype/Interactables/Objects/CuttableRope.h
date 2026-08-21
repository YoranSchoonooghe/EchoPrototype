// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CuttableRope.generated.h"

class UStaticMeshComponent;
class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRopeCut);

UCLASS()
class ECHOPROTOTYPE_API ACuttableRope : public AActor
{
	GENERATED_BODY()

public:
	ACuttableRope();

	UPROPERTY(BlueprintAssignable, Category = "Rope")
	FOnRopeCut OnRopeCut;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RopeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rope", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Rope")
	void OnRopeSevered();

private:
	UFUNCTION()
	void HandleRopeHealthDepleted();
};
