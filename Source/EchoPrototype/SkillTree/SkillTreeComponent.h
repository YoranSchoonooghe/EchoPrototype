#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SkillTreeComponent.generated.h"

class USkillTreeNodeData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillTreeComponent();

	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	FORCEINLINE int32 GetAvailableSkillPoints() const { return AvailableSkillPoints; }

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void AddSkillPoints(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool IsUnlocked(USkillTreeNodeData* Node) const;

	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool CanUnlock(USkillTreeNodeData* Node) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool TryUnlock(USkillTreeNodeData* Node);

	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool HasUnlockedTag(FGameplayTag Tag) const { return UnlockedAbilityTags.HasTag(Tag); }

	void UnlockTag(const FGameplayTag& Tag) { UnlockedAbilityTags.AddTag(Tag); }

protected:
	UPROPERTY(EditAnywhere, Category = "Skill Tree")
	int32 AvailableSkillPoints = 0;

private:
	UPROPERTY()
	TArray<TObjectPtr<USkillTreeNodeData>> UnlockedNodes;

	FGameplayTagContainer UnlockedAbilityTags;
};
