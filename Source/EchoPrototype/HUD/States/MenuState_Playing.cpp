#include "MenuState_Playing.h"

#include "../MenuFlowSubsystem.h"

void UMenuState_Playing::OnBackPressedAtRoot(UMenuFlowSubsystem* Flow)
{
	if (Flow && PausedStateClass)
	{
		Flow->PushState(PausedStateClass);
	}
}
