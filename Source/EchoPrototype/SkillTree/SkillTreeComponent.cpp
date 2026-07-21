#include "SkillTreeComponent.h"

#include "Effects/SkillEffect.h"
#include "SkillTreeNodeData.h"
#include "../Character/PlayerCharacter.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillTreeComponent::AddSkillPoints(int32 Amount)
{
	AvailableSkillPoints = FMath::Max(0, AvailableSkillPoints + Amount);
}

bool USkillTreeComponent::IsUnlocked(USkillTreeNodeData* Node) const
{
	return Node && UnlockedNodes.Contains(Node);
}

bool USkillTreeComponent::CanUnlock(USkillTreeNodeData* Node) const
{
	if (!Node || IsUnlocked(Node) || AvailableSkillPoints < Node->Cost)
	{
		return false;
	}

	for (USkillTreeNodeData* Prerequisite : Node->Prerequisites)
	{
		if (Prerequisite && !IsUnlocked(Prerequisite))
		{
			return false;
		}
	}

	return true;
}

bool USkillTreeComponent::TryUnlock(USkillTreeNodeData* Node)
{
	if (!CanUnlock(Node))
	{
		return false;
	}

	AvailableSkillPoints -= Node->Cost;
	UnlockedNodes.Add(Node);
	ApplyNodeEffects(Node);

	return true;
}

TArray<USkillTreeNodeData*> USkillTreeComponent::GetUnlockedNodes() const
{
	TArray<USkillTreeNodeData*> Result;
	Result.Reserve(UnlockedNodes.Num());

	for (USkillTreeNodeData* Node : UnlockedNodes)
	{
		Result.Add(Node);
	}

	return Result;
}

void USkillTreeComponent::LoadUnlockedNodes(int32 SavedSkillPoints, const TArray<USkillTreeNodeData*>& SavedUnlockedNodes)
{
	AvailableSkillPoints = SavedSkillPoints;
	UnlockedNodes.Reset();
	UnlockedAbilityTags.Reset();

	for (USkillTreeNodeData* Node : SavedUnlockedNodes)
	{
		if (!Node)
		{
			continue;
		}

		UnlockedNodes.Add(Node);
		ApplyNodeEffects(Node);
	}
}

void USkillTreeComponent::ApplyNodeEffects(USkillTreeNodeData* Node)
{
	if (!Node)
	{
		return;
	}

	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
	for (USkillEffect* Effect : Node->Effects)
	{
		if (Effect)
		{
			Effect->Apply(Character);
		}
	}
}
