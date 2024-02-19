// Copyright Epic Games, Inc. All Rights Reserved.

#include "DetailCustomizations/CompositePlanarReflectionDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"

TSharedRef<IDetailCustomization> FCompositePlanarReflectionDetails::MakeInstance()
{
	return MakeShareable(new FCompositePlanarReflectionDetails);
}

void FCompositePlanarReflectionDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.HideCategory("Projection");
	DetailBuilder.HideCategory("PostProcessVolume");
	DetailBuilder.HideCategory("Lens");
	DetailBuilder.HideCategory("Color Grading");
	DetailBuilder.HideCategory("Rendering Features");
	DetailBuilder.HideCategory("Rendering");
	DetailBuilder.HideCategory("Tags");
	DetailBuilder.HideCategory("Activation");
	DetailBuilder.HideCategory("Cooking");
	DetailBuilder.HideCategory("Replication");
	DetailBuilder.HideCategory("Input");
	DetailBuilder.HideCategory("PlanarReflection");
	DetailBuilder.HideCategory("LOD");
	DetailBuilder.HideCategory("AssetUserData");
	DetailBuilder.HideCategory("Physics");
	DetailBuilder.HideCategory("SceneCapture");

	// Hack, add category with underscore in front of it so it looks the same as the component category, but we hide that one.
	IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("_SceneCapture");
	TSharedPtr<IPropertyHandle> PropertyHandle = DetailBuilder.GetProperty("CompositePlanarReflectionComponent");
	if (PropertyHandle->IsValidHandle())
	{
		TSharedPtr<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle("ShowOnlyActors");
		if (ChildHandle->IsValidHandle())
		{
			CustomCategory.AddProperty(ChildHandle);
		}
	}
}
