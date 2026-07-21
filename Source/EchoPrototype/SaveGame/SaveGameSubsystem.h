#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

class APlayerCharacter;
class UEchoSaveGame;

UCLASS()
class ECHOPROTOTYPE_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGame(APlayerCharacter* Character, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadGame(const FString& SlotName);

	UFUNCTION(BlueprintPure, Category = "Save")
	bool DoesSaveExist(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool DeleteSave(const FString& SlotName) const;

	void ApplyPendingLoadIfAny(APlayerCharacter* Character);

private:
	UPROPERTY()
	TObjectPtr<UEchoSaveGame> PendingLoad;
};
