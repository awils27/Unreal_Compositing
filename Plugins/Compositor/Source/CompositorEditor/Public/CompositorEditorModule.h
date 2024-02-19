// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"

class FToolBarBuilder;
class FMenuBuilder;
class UWidgetBlueprint;
class UEditorUtilityWidget;

class FCompositorEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static EAssetTypeCategories::Type GetAssetCategory() { return CompositorAssetCategory; }

	void OnLevelActorDeleted(AActor* Actor);

private:
	void RegisterMenus();

	TSharedPtr<class FUICommandList> PluginCommands;

	void RegisterPlacementModeExtensions();

	void UnregisterPlacementModeExtensions();

	void SubscribeEvents();

	void UnsubscribeEvents();

	static EAssetTypeCategories::Type CompositorAssetCategory;

	FDelegateHandle OnLevelActorDeletedDelegateHandle;
};
