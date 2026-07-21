#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EchoSaveGame.generated.h"

class USkillTreeNodeData;

UCLASS()
class ECHOPROTOTYPE_API UEchoSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Save")
	int32 SkillPoints = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	TArray<TSoftObjectPtr<USkillTreeNodeData>> UnlockedNodes;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FName LevelName;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FRotator PlayerRotation = FRotator::ZeroRotator;
};
