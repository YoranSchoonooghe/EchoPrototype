// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveZone.generated.h"

class UBoxComponent;

UCLASS()
class ECHOPROTOTYPE_API ASaveZone : public AActor
{
	GENERATED_BODY()

public:
	ASaveZone();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Zone", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> SaveVolume;

	UPROPERTY(EditAnywhere, Category = "Save Zone")
	FString SaveSlotName = TEXT("Save0");

	UFUNCTION(BlueprintImplementableEvent, Category = "Save Zone")
	void OnGameSaved();

private:
	UFUNCTION()
	void OnSaveVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
