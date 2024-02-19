// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CompositorEditorUtilityWidgetStyle.h"

class FCompositorEditorUtilityWidgetCommands : public TCommands<FCompositorEditorUtilityWidgetCommands>
{
public:

	FCompositorEditorUtilityWidgetCommands()
		: TCommands<FCompositorEditorUtilityWidgetCommands>(TEXT("CompositorEditorUtilityWidget"), NSLOCTEXT("Contexts", "CompositorEditorUtilityWidget", "CompositorEditorUtilityWidget Plugin"), NAME_None, FCompositorEditorUtilityWidgetStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenCompositorEditor;
};
