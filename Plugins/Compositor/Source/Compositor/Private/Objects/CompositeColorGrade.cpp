// Copyright Epic Games, Inc. All Rights Reserved.

#include "Objects/CompositeColorGrade.h"
#include "Assets/Composite.h"

UCompositeColorGrade::UCompositeColorGrade()
{
	CompositeColorGradeClassDefaults = Cast<UCompositeColorGrade>(UCompositeColorGrade::StaticClass()->GetDefaultObject(true));
}

FColorGradePerRangeSettings UCompositeColorGrade::GetColorGradeScene() const
{
	UComposite* Composite = Cast<UComposite>(GetOuter());
	if (Composite)
	{
		if (bOverride_ColorGradeScene)
		{
			return ColorGradeScene;
		}

		if (UComposite* ParentComposite = Composite->GetParentComposite())
		{
			if (UCompositeColorGrade* ParentCompositeColorGrade = ParentComposite->GetCompositeColorGrade())
			{
				return ParentCompositeColorGrade->GetColorGradeScene();
			}
		}
	}

	return CompositeColorGradeClassDefaults->ColorGradeScene;
}

void UCompositeColorGrade::SetColorGradeScene(FColorGradePerRangeSettings NewColorGradeScene)
{
	ColorGradeScene = NewColorGradeScene;
}

FColorGradePerRangeSettings UCompositeColorGrade::GetColorGradeMedia() const
{
	UComposite* Composite = Cast<UComposite>(GetOuter());
	if (Composite)
	{
		if (bOverride_ColorGradeMedia)
		{
			return ColorGradeMedia;
		}

		if (UComposite* ParentComposite = Composite->GetParentComposite())
		{
			if (UCompositeColorGrade* ParentCompositeColorGrade = ParentComposite->GetCompositeColorGrade())
			{
				return ParentCompositeColorGrade->GetColorGradeMedia();
			}
		}
	}

	return CompositeColorGradeClassDefaults->ColorGradeMedia;
}

void UCompositeColorGrade::SetColorGradeMedia(FColorGradePerRangeSettings NewColorGradeMedia)
{
	ColorGradeMedia = NewColorGradeMedia;
}

FColorGradePerRangeSettings UCompositeColorGrade::GetColorGradeCombined() const
{
	UComposite* Composite = Cast<UComposite>(GetOuter());
	if (Composite)
	{
		if (bOverride_ColorGradeCombined)
		{
			return ColorGradeCombined;
		}

		if (UComposite* ParentComposite = Composite->GetParentComposite())
		{
			if (UCompositeColorGrade* ParentCompositeColorGrade = ParentComposite->GetCompositeColorGrade())
			{
				return ParentCompositeColorGrade->GetColorGradeCombined();
			}
		}
	}

	return CompositeColorGradeClassDefaults->ColorGradeCombined;
}

void UCompositeColorGrade::SetColorGradeCombined(FColorGradePerRangeSettings NewColorGradeCombined)
{
	ColorGradeCombined = NewColorGradeCombined;
}
