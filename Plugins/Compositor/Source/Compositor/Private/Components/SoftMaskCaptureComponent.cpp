// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/SoftMaskCaptureComponent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Interfaces/CompositeUpdateInterface.h"
#include "Actors/CompositeMesh.h"
#include "Assets/Composite.h"

USoftMaskCaptureComponent::USoftMaskCaptureComponent()
{
	TextureTarget = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/SoftMask/RT_CompositorSoftMask.RT_CompositorSoftMask")).TryLoad());
	
	ProfilingEventName = "Composite Soft Mask";

	ShowFlags.Landscape = false;
	ShowFlags.InstancedStaticMeshes = false;
	ShowFlags.TextRender = false;

	bAllowDebugEditorCamera = true;
}

bool USoftMaskCaptureComponent::GetIsCompositeCaptureActive() const
{
	const bool HasAnyComponentsToCapture = ShowOnlyComponents.Num() > 0;

	if (WorldComposite)
	{
		return WorldComposite->GetEnableSoftMask() && HasAnyComponentsToCapture;
	}

	return HasAnyComponentsToCapture;
}

void USoftMaskCaptureComponent::OnCompositeUpdateInterfaceRegistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface)
{
	ACompositeMesh* CompositeMesh = Cast<ACompositeMesh>(CompositeUpdateInterface.GetObject());
	if (CompositeMesh != nullptr)// && CompositeMesh->GetRenderSoftMask() != ERenderSoftMaskType::Off)
	{
		ShowOnlyComponents.Add(CompositeMesh->GetSoftMaskComponent());
	}
}

void USoftMaskCaptureComponent::OnCompositeUpdateInterfaceUnregistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface)
{
	ACompositeMesh* CompositeMesh = Cast<ACompositeMesh>(CompositeUpdateInterface.GetObject());
	if (CompositeMesh != nullptr)// && CompositeMesh->GetRenderSoftMask() != ERenderSoftMaskType::Off)
	{
		ShowOnlyComponents.Remove(CompositeMesh->GetSoftMaskComponent());
	}
}

float USoftMaskCaptureComponent::GetTargetTextureScreenPercentage() const
{
	if (WorldComposite)
	{
		return WorldComposite->GetSoftMaskScreenPercentage();
	}

	return Super::GetTargetTextureScreenPercentage();
}
