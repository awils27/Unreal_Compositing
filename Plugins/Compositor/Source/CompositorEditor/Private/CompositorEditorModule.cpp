// Copyright Epic Games, Inc. All Rights Reserved.

#include "CompositorEditorModule.h"
#include "CompositorEditorStyle.h"
#include "CompositorEditorCommands.h"
#include "AssetTypeActions/AssetTypeActions_CompositeKeyer.h"
#include "AssetTypeActions/AssetTypeActions_Composite.h"
#include "Actors/CompositeMesh.h"
#include "Subsystems/CompositorSubsystem.h"
#include "Actors/CompositePlanarReflection.h"
#include "DetailCustomizations/CompositePlanarReflectionDetails.h"
#include "DetailCustomizations/CompositorWorldSettingsDetails.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"

#include "LevelEditor.h"
#include "ToolMenus.h"

#include "IPlacementModeModule.h"
#include "ActorFactories/ActorFactoryBlueprint.h"
#include "Blutility/Classes/EditorUtilityWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/WorldSettings.h"

static const FName CompositorEditorTabName("Compositor");

EAssetTypeCategories::Type FCompositorEditorModule::CompositorAssetCategory;

#define LOCTEXT_NAMESPACE "FCompositorEditorModule"

void FCompositorEditorModule::StartupModule()
{	
	FCompositorEditorStyle::Initialize();
	FCompositorEditorStyle::ReloadTextures();

	FCompositorEditorCommands::Register();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCompositorEditorModule::RegisterMenus));

    IAssetTools &AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    CompositorAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Compositor")), LOCTEXT("CompositorCategory", "Compositor"));
    {
		TSharedRef<IAssetTypeActions> CompositeAssetTypeActions = MakeShareable(new FAssetTypeActions_Composite);
		AssetTools.RegisterAssetTypeActions(CompositeAssetTypeActions);

        TSharedRef<IAssetTypeActions> CompositeKeyerAssetTypeActions = MakeShareable(new FAssetTypeActions_CompositeKeyer);
        AssetTools.RegisterAssetTypeActions(CompositeKeyerAssetTypeActions);
    }

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(ACompositePlanarReflection::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCompositePlanarReflectionDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(AWorldSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCompositorWorldSettingsDetails::MakeInstance));


	RegisterPlacementModeExtensions();

	SubscribeEvents();
}

void FCompositorEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCompositorEditorStyle::Shutdown();

	FCompositorEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CompositorEditorTabName);

	UnregisterPlacementModeExtensions();

	UnsubscribeEvents();
}

void FCompositorEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	//FToolMenuOwnerScoped OwnerScoped(this);

	//{
	//	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	//	{
	//		FToolMenuSection& Section = Menu->FindOrAddSection("Level Editor");
	//		Section.AddMenuEntryWithCommandList(FCompositorEditorCommands::Get().OpenPluginWindow, PluginCommands);
	//	}
	//}

	//{
	//	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
	//	{
	//		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Misc");
	//		{
	//			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCompositorEditorCommands::Get().OpenPluginWindow));
	//			Entry.SetCommandList(PluginCommands);
	//		}
	//	}
	//}
}

void FCompositorEditorModule::RegisterPlacementModeExtensions()
{	
	FPlacementCategoryInfo CategoryInfo(LOCTEXT("PlacementMode_Compositor", "Compositor"), FSlateIcon(FAppStyle::Get().GetStyleSetName(), TEXT("LevelEditor.Tabs.Layers")), "Compositor", TEXT("PMCompositor"), 46);

	IPlacementModeModule::Get().RegisterPlacementCategory(CategoryInfo);

	IPlacementModeModule::Get().RegisterPlaceableItem(CategoryInfo.UniqueHandle, MakeShareable( new FPlaceableItem(nullptr, FAssetData(ACompositeMesh::StaticClass())) ));
	IPlacementModeModule::Get().RegisterPlaceableItem(CategoryInfo.UniqueHandle, MakeShareable(new FPlaceableItem(nullptr, FAssetData(ACompositePlanarReflection::StaticClass()))));
}

void FCompositorEditorModule::UnregisterPlacementModeExtensions()
{
	if (IPlacementModeModule::IsAvailable())
	{
		IPlacementModeModule::Get().UnregisterPlacementCategory("PMCompositor");
	}
}

void FCompositorEditorModule::SubscribeEvents()
{
	UnsubscribeEvents();
	OnLevelActorDeletedDelegateHandle = GEngine->OnLevelActorDeleted().AddRaw(this, &FCompositorEditorModule::OnLevelActorDeleted);
}

void FCompositorEditorModule::UnsubscribeEvents()
{
	if (GEngine->IsValidLowLevel()) // Needs to be check to avoid crash on close of the editor.
	{
		GEngine->OnLevelActorDeleted().Remove(OnLevelActorDeletedDelegateHandle);
	}
}

void FCompositorEditorModule::OnLevelActorDeleted(AActor* Actor)
{
	ACompositeMesh* CompositeMesh = Cast<ACompositeMesh>(Actor);
	if (CompositeMesh != nullptr)
	{
		UCompositorSubsystem* CompositorSubsystem = CompositeMesh->GetWorld()->GetSubsystem<UCompositorSubsystem>();
		if (CompositorSubsystem != nullptr)
		{
			CompositorSubsystem->UnregisterCompositeUpdateInterface(CompositeMesh);
		}			
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCompositorEditorModule, CompositorEditor)