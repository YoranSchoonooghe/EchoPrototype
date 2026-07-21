#pragma once

#include "CoreMinimal.h"
#include "SkillEffect.h"
#include "SE_ModifyStat.generated.h"

UENUM(BlueprintType)
enum class ESkillStat : uint8
{
	MaxHealth,
	AttackDamage,
	SprintSpeed,
	WalkSpeed,
	SneakSpeed
};

UCLASS()
class ECHOPROTOTYPE_API USE_ModifyStat : public USkillEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	ESkillStat Stat = ESkillStat::MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modify Stat")
	float Delta = 0.0f;

	virtual void Apply(APlayerCharacter* Character) override;
};
