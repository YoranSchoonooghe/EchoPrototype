#include "SE_ModifyStat.h"

#include "../../Character/PlayerCharacter.h"
#include "../../Combat/CombatComponent.h"
#include "../../Combat/HealthComponent.h"

void USE_ModifyStat::Apply(APlayerCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	switch (Stat)
	{
	case ESkillStat::MaxHealth:
		if (UHealthComponent* Health = Character->GetHealthComponent())
		{
			Health->AddMaxHealthBonus(Delta);
		}
		break;

	case ESkillStat::AttackDamage:
		if (UCombatComponent* Combat = Character->GetCombatComponent())
		{
			Combat->AddAttackDamageMultiplierBonus(Delta);
		}
		break;

	case ESkillStat::SprintSpeed:
		Character->AddSprintSpeedBonus(Delta);
		break;

	case ESkillStat::WalkSpeed:
		Character->AddWalkSpeedBonus(Delta);
		break;

	case ESkillStat::SneakSpeed:
		Character->AddSneakSpeedBonus(Delta);
		break;
	}
}
