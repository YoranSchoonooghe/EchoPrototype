#include "SE_UnlockAbilityTag.h"

#include "../../Character/PlayerCharacter.h"
#include "../SkillTreeComponent.h"

void USE_UnlockAbilityTag::Apply(APlayerCharacter* Character)
{
	if (!Character || !AbilityTag.IsValid())
	{
		return;
	}

	if (USkillTreeComponent* SkillTree = Character->GetSkillTreeComponent())
	{
		SkillTree->UnlockTag(AbilityTag);
	}
}
