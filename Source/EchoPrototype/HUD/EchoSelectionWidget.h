// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EchoSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECHOPROTOTYPE_API UEchoSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Echo", meta = (ExposeOnSpawn = "true"))
	AActor* TargetEchoActor;
};
