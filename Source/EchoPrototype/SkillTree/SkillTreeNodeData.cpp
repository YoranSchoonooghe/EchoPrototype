#include "SkillTreeNodeData.h"

void USkillTreeNodeData::SetTreePosition(const FVector2D& NewPosition)
{
	Modify();
	TreePosition = NewPosition;
}
