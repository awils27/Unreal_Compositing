// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions/AssetTypeActions_Composite.h"

#include "Assets/Composite.h"

//#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_Composite::GetSupportedClass() const
{
	return UComposite::StaticClass();
}

void FAssetTypeActions_Composite::GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section)
{
	auto Composites = GetTypedWeakObjectPtrs<UComposite>(InObjects);
}

void FAssetTypeActions_Composite::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>() */)
{
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

//#undef LOCTEXT_NAMESPACE
