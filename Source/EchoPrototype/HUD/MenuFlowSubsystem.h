#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MenuFlowSubsystem.generated.h"

class UMenuStateBase;
class APlayerController;

UCLASS()
class ECHOPROTOTYPE_API UMenuFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void SetRootState(TSubclassOf<UMenuStateBase> StateClass);

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void PushState(TSubclassOf<UMenuStateBase> StateClass);

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void PopState();

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void RequestBack();

	UFUNCTION(BlueprintPure, Category = "Menu Flow")
	UMenuStateBase* GetCurrentState() const;

	APlayerController* GetFirstPlayerController() const;

private:
	UPROPERTY()
	TArray<TObjectPtr<UMenuStateBase>> StateStack;
};
