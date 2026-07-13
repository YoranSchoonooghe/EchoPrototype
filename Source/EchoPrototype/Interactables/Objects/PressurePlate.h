// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"


class UStaticMeshComponent;
class UBoxComponent;
class ADoor;


UCLASS()
class ECHOPROTOTYPE_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure Plate", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlateMesh;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure Plate", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> PressureZone;


	UPROPERTY(EditInstanceOnly, Category = "Pressure Plate")
	TObjectPtr<ADoor> LinkedDoor;


	UPROPERTY(EditAnywhere, Category = "Pressure Plate")
	float PressedDepth = 5.0f;


	UPROPERTY(EditAnywhere, Category = "Pressure Plate")
	bool bOnlyPawnsTrigger = true;

	UPROPERTY(EditAnywhere, Category = "Pressure Plate")
	bool bOnlyEchoTriggers = true;

	UFUNCTION()
	void OnPressureZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPressureZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bIsTransitioning = false;

	FTimerHandle ResetTransitionTimerHandle;

	void ClearTransitionGate();

private:	
	
	UPROPERTY(Transient)
	TSet<TObjectPtr<AActor>> OverlappingActors;

	FVector RestingRelativeLocation = FVector::ZeroVector;

	bool IsValidTrigger(AActor* OtherActor) const;

};
