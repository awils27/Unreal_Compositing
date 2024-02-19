// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions/AssetTypeActions_CompositeKeyer.h"

#include "Assets/CompositeKeyer.h"

//#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_CompositeKeyer::GetSupportedClass() const
{
	return UCompositeKeyer::StaticClass();
}

void FAssetTypeActions_CompositeKeyer::GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section)
{
	auto CompositeKeyers = GetTypedWeakObjectPtrs<UCompositeKeyer>(InObjects);
}

void FAssetTypeActions_CompositeKeyer::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>() */)
{
	struct Local
	{
		static TArray<UObject*> GetSubObjects(const TArray<UObject*>& InObjects)
		{
			TArray<UObject*> SubObjects;
			for(UObject* Object : InObjects)
			{
				auto CompositeKeyer = Cast<UCompositeKeyer>(Object);
				if(CompositeKeyer)
				{
					SubObjects.Add(CompositeKeyer);
				}
			}
			return SubObjects;
		}
	};

	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects, FSimpleAssetEditor::FGetDetailsViewObjects::CreateStatic(&Local::GetSubObjects));
}

//#undef LOCTEXT_NAMESPACE
