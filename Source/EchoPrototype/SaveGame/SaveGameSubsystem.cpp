#include "SaveGameSubsystem.h"

#include "EchoSaveGame.h"
#include "../Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void USaveGameSubsystem::SaveGame(APlayerCharacter* Character, const FString& SlotName)
{
	if (!Character)
	{
		return;
	}

	if (UEchoSaveGame* SaveGameObject = Character->CaptureSaveGame())
	{
		UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
	}
}

void USaveGameSubsystem::LoadGame(const FString& SlotName)
{
	UEchoSaveGame* Loaded = Cast<UEchoSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!Loaded)
	{
		return;
	}

	PendingLoad = Loaded;

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World, true);

	if (!Loaded->LevelName.IsNone() && Loaded->LevelName.ToString() != CurrentLevelName)
	{
		UGameplayStatics::OpenLevel(World, Loaded->LevelName);
		return;
	}

	if (APlayerCharacter* Character = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0)))
	{
		ApplyPendingLoadIfAny(Character);
	}
}

bool USaveGameSubsystem::DoesSaveExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

bool USaveGameSubsystem::DeleteSave(const FString& SlotName) const
{
	return UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}

void USaveGameSubsystem::ApplyPendingLoadIfAny(APlayerCharacter* Character)
{
	if (!PendingLoad || !Character)
	{
		return;
	}

	Character->ApplySaveGame(PendingLoad);
	PendingLoad = nullptr;
}
