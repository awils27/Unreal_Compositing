// Copyright Epic Games, Inc. All Rights Reserved.

#include "CompositorEditorCommands.h"

#define LOCTEXT_NAMESPACE "FCompositorEditorModule"

void FCompositorEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Compositor", "Open up Compositor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
