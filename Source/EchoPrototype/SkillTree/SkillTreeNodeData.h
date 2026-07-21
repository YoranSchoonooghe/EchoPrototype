#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Effects/SkillEffect.h"
#include "SkillTreeNodeData.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class ECHOPROTOTYPE_API USkillTreeNodeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int32 Cost = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TArray<TObjectPtr<USkillTreeNodeData>> Prerequisites;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Skill")
	TArray<TObjectPtr<USkillEffect>> Effects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Layout")
	FVector2D TreePosition = FVector2D::ZeroVector;

	UFUNCTION(BlueprintCallable, Category = "Layout")
	void SetTreePosition(const FVector2D& NewPosition);
};
