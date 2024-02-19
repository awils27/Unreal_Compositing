// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CompositorEditorStyle.h"

class FCompositorEditorCommands : public TCommands<FCompositorEditorCommands>
{
public:

	FCompositorEditorCommands()
		: TCommands<FCompositorEditorCommands>(TEXT("CompositorEditor"), NSLOCTEXT("Contexts", "CompositorEditor", "CompositorEditor Plugin"), NAME_None, FCompositorEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};