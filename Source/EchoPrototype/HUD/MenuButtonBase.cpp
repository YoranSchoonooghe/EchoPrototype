#include "MenuButtonBase.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"

UMenuButtonBase::UMenuButtonBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnHovered.AddDynamic(this, &UMenuButtonBase::HandleHovered);
	OnUnhovered.AddDynamic(this, &UMenuButtonBase::HandleUnhovered);
	OnReceivedFocus.BindUObject(this, &UMenuButtonBase::HandleReceivedFocus);
	OnLostFocus.BindUObject(this, &UMenuButtonBase::HandleLostFocus);

	SetRenderTransformPivot(FVector2D(0.5f, 0.5f));

	AnimationTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UMenuButtonBase::TickAnimation));

	static bool bHasClearedFocusBrush = false;
	if (!bHasClearedFocusBrush)
	{
		bHasClearedFocusBrush = true;

		if (FSlateBrush* FocusBrush = const_cast<FSlateBrush*>(FCoreStyle::Get().GetBrush("FocusRectangle")))
		{
			FocusBrush->DrawAs = ESlateBrushDrawType::NoDrawType;
		}
	}
}

void UMenuButtonBase::BeginDestroy()
{
	FTSTicker::GetCoreTicker().RemoveTicker(AnimationTickerHandle);

	Super::BeginDestroy();
}

void UMenuButtonBase::HandleHovered()
{
	bIsMouseHovered = true;
	RefreshBumpState();
}

void UMenuButtonBase::HandleUnhovered()
{
	bIsMouseHovered = false;
	RefreshBumpState();
}

void UMenuButtonBase::HandleReceivedFocus()
{
	bIsFocused = true;
	RefreshBumpState();
	RefreshVisualStyle();
}

void UMenuButtonBase::HandleLostFocus()
{
	bIsFocused = false;
	RefreshBumpState();
	RefreshVisualStyle();
}

void UMenuButtonBase::RefreshBumpState()
{
	const bool bBumped = bIsMouseHovered || bIsFocused;

	TargetScale = bBumped ? FVector2D(BumpScale, BumpScale) : FVector2D::UnitVector;
	TargetAngle = bBumped ? BumpAngle : 0.f;
}

void UMenuButtonBase::RefreshVisualStyle()
{
	if (bIsFocused)
	{
		if (!bHasSavedNormalBrush)
		{
			SavedNormalBrush = GetStyle().Normal;
			bHasSavedNormalBrush = true;
		}

		FButtonStyle Style = GetStyle();
		Style.Normal = Style.Hovered;
		SetStyle(Style);
	}
	else if (bHasSavedNormalBrush)
	{
		FButtonStyle Style = GetStyle();
		Style.Normal = SavedNormalBrush;
		SetStyle(Style);
		bHasSavedNormalBrush = false;
	}
}

bool UMenuButtonBase::TickAnimation(float DeltaTime)
{
	const FVector2D NewScale = FMath::Vector2DInterpTo(GetRenderTransform().Scale, TargetScale, DeltaTime, BumpInterpSpeed);
	const float NewAngle = FMath::FInterpTo(GetRenderTransformAngle(), TargetAngle, DeltaTime, BumpInterpSpeed);

	SetRenderScale(NewScale);
	SetRenderTransformAngle(NewAngle);

	return true;
}
