// Copyright Epic Games, Inc. All Rights Reserved.

#include "CompositorEditorUtilityWidget.h"
#include "CompositorEditorUtilityWidgetStyle.h"
#include "CompositorEditorUtilityWidgetCommands.h"
#include "ToolMenus.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "LevelEditor.h"

static const FName CompositorEditorUtilityWidgetTabName("CompositorEditorUtilityWidget");

#define LOCTEXT_NAMESPACE "FCompositorEditorUtilityWidgetModule"

static TSharedRef<SDockTab> SpawnCompositorTab(const FSpawnTabArgs& InSpawnTabArgs)
{
	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	return NewTab;
}

void FCompositorEditorUtilityWidgetModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FCompositorEditorUtilityWidgetStyle::Initialize();
	FCompositorEditorUtilityWidgetStyle::ReloadTextures();

	FCompositorEditorUtilityWidgetCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCompositorEditorUtilityWidgetCommands::Get().OpenCompositorEditor,
		FExecuteAction::CreateRaw(this, &FCompositorEditorUtilityWidgetModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCompositorEditorUtilityWidgetModule::RegisterMenus));
}

void FCompositorEditorUtilityWidgetModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FCompositorEditorUtilityWidgetStyle::Shutdown();

	FCompositorEditorUtilityWidgetCommands::Unregister();
}

void FCompositorEditorUtilityWidgetModule::PluginButtonClicked()
{
	UEditorUtilityWidgetBlueprint* CompositorWidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(FSoftObjectPath(TEXT("/Compositor/Editor/Compositor.Compositor")).TryLoad());

	if (CompositorWidgetBlueprint != nullptr)
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		EditorUtilitySubsystem->SpawnAndRegisterTab(CompositorWidgetBlueprint);
	}
}

void FCompositorEditorUtilityWidgetModule::RegisterMenus()
{
	//// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	//FToolMenuOwnerScoped OwnerScoped(this);
	//{
	//	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.Cinematics");
	//	{
	//		FToolMenuInsert MenuInsert;
	//		MenuInsert.Position = EToolMenuInsertType::First;
	//		FToolMenuSection& Section = ToolbarMenu->AddSection("Compositor", LOCTEXT("Compositor", "Compositor"), MenuInsert);
	//		{
	//			Section.AddMenuEntryWithCommandList(FCompositorEditorUtilityWidgetCommands::Get().OpenCompositorEditor, PluginCommands);
	//		}
	//	}
	//}


	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Compositor");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCompositorEditorUtilityWidgetCommands::Get().OpenCompositorEditor));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCompositorEditorUtilityWidgetModule, CompositorEditorUtilityWidget)