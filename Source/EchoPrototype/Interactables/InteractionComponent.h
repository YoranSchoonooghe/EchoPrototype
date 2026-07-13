// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusedActorChanged, AActor*, NewFocusedActor, FText, InteractionPrompt);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOPROTOTYPE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteractPressed();


	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetCurrentFocusedActor() const { return CurrentFocusedActor.Get(); }

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusedActorChanged OnFocusedActorChanged;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionDistance = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRadius = 10.0f;

private:
	TWeakObjectPtr<AActor> CurrentFocusedActor;

	void UpdateFocus();

	APawn* GetOwnerPawn() const;

	FVector EchoViewLocation;
	FRotator EchoViewRotation;
};
