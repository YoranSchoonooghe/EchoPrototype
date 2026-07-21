#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SkillEffect.h"
#include "SE_UnlockAbilityTag.generated.h"

UCLASS()
class ECHOPROTOTYPE_API USE_UnlockAbilityTag : public USkillEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
	FGameplayTag AbilityTag;

	virtual void Apply(APlayerCharacter* Character) override;
};
