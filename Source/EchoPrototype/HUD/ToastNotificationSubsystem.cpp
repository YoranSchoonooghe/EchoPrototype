#include "ToastNotificationSubsystem.h"

void UToastNotificationSubsystem::ShowToast(UTexture2D* Icon, const FText& Title, const FText& Description)
{
	FToastNotificationData Toast;
	Toast.Icon = Icon;
	Toast.Title = Title;
	Toast.Description = Description;

	PendingToasts.Add(Toast);

	TryShowNext();
}

void UToastNotificationSubsystem::NotifyToastFinished()
{
	bIsShowingToast = false;
	TryShowNext();
}

void UToastNotificationSubsystem::TryShowNext()
{
	if (bIsShowingToast || PendingToasts.Num() == 0)
	{
		return;
	}

	bIsShowingToast = true;

	const FToastNotificationData Toast = PendingToasts[0];
	PendingToasts.RemoveAt(0);

	OnToastShown.Broadcast(Toast);
}
