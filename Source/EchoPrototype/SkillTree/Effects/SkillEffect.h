#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SkillEffect.generated.h"

class APlayerCharacter;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ECHOPROTOTYPE_API USkillEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void Apply(APlayerCharacter* Character) PURE_VIRTUAL(USkillEffect::Apply, );
};
