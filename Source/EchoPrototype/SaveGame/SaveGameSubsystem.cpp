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
		SaveGameObject->CollectedPickups = CollectedPickupIDs.Array();
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

	CollectedPickupIDs = TSet<FName>(Loaded->CollectedPickups);
	PendingLoad = Loaded;

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	const FName LevelToOpen = !Loaded->LevelName.IsNone() ? Loaded->LevelName : FName(*UGameplayStatics::GetCurrentLevelName(World, true));
	UGameplayStatics::OpenLevel(World, LevelToOpen);
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

bool USaveGameSubsystem::IsPickupCollected(FName PickupID) const
{
	return CollectedPickupIDs.Contains(PickupID);
}

void USaveGameSubsystem::MarkPickupCollected(FName PickupID)
{
	CollectedPickupIDs.Add(PickupID);
}
