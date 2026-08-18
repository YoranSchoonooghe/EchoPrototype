#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ToastNotificationSubsystem.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FToastNotificationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Toast")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Toast")
	FText Title;

	UPROPERTY(BlueprintReadOnly, Category = "Toast")
	FText Description;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToastShown, const FToastNotificationData&, Toast);

UCLASS()
class ECHOPROTOTYPE_API UToastNotificationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Toast")
	void ShowToast(UTexture2D* Icon, const FText& Title, const FText& Description);

	UFUNCTION(BlueprintCallable, Category = "Toast")
	void NotifyToastFinished();

	UPROPERTY(BlueprintAssignable, Category = "Toast")
	FOnToastShown OnToastShown;

private:
	void TryShowNext();

	TArray<FToastNotificationData> PendingToasts;
	bool bIsShowingToast = false;
};
