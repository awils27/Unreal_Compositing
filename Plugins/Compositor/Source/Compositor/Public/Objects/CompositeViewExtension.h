// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SceneViewExtension.h"

class UCompositorSubsystem;

/**
 *
 */
class FCompositeViewExtension : public FSceneViewExtensionBase
{
public:
	FCompositeViewExtension(const FAutoRegister& AutoRegister, UCompositorSubsystem* Owner);

public:
	//~ ISceneViewExtension interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {}
	virtual int32 GetPriority() const override;

protected:
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;

private:
	TWeakObjectPtr<UCompositorSubsystem> CompositorSubsystem;
};