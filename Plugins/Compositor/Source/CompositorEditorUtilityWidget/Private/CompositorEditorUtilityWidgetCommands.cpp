// Copyright Epic Games, Inc. All Rights Reserved.

#include "CompositorEditorUtilityWidgetCommands.h"

#define LOCTEXT_NAMESPACE "FCompositorEditorUtilityWidgetModule"

void FCompositorEditorUtilityWidgetCommands::RegisterCommands()
{
	UI_COMMAND(OpenCompositorEditor, "Compositor", "Open the Compositor editor.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
