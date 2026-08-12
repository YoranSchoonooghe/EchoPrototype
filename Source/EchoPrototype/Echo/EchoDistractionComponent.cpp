// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoDistractionComponent.h"

UEchoDistractionComponent::UEchoDistractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsDetectable = true;
}

void UEchoDistractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEchoDistractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

