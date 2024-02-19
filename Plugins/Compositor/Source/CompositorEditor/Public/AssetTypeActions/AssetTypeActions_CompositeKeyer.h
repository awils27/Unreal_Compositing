// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
#include "CompositorEditorModule.h"

class FAssetTypeActions_CompositeKeyer : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
    virtual FText GetName() const override { return FText::FromString("Compositor CompositeKeyer"); };// { return LOCTEXT("MyDataAsset", "My Items Data Asset"); }
    virtual FColor GetTypeColor() const override { return FColor(40,166,50); }
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override { return FCompositorEditorModule::GetAssetCategory(); }
	virtual bool HasActions ( const TArray<UObject*>& InObjects ) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;

	virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>() ) override;

    virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("A CompositeKeyer which can be used for the compositor."); };// { return LOCTEXT("MyDataAssetDesc", "My DataAsset description."); }
};