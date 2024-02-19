// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Scene.h" // FPostProcessSettings
#include "Interfaces/Interface_PostProcessVolume.h"
#include "Materials/MaterialInterface.h"

struct FCompositePostProcessVolume : public IInterface_PostProcessVolume
{
	FCompositePostProcessVolume()
		: PostProcessProperties()
	{
		bDebugVisualizeCompositeMeshes = false;
		bDebugVisualizeShadows = false;
		
		PostProcessProperties.bIsEnabled = true;
		PostProcessProperties.bIsUnbound = true;
		PostProcessProperties.BlendWeight = 1.F;
		PostProcessProperties.Priority = 1000;
		
		BeforeTranslucencyMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_BeforeTranslucency.MI_Compositor_BeforeTranslucency")).TryLoad());
		SsrInputMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_SsrInput.MI_Compositor_SsrInput")).TryLoad());
		AfterTonemappingMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_AfterTonemapping.MI_Compositor_AfterTonemapping")).TryLoad());

		DebugVisualizeCompositeMeshesBeforeTranslucencyMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_DebugVisualizeCompositeMeshes_BeforeTranslucency.MI_Compositor_DebugVisualizeCompositeMeshes_BeforeTranslucency")).TryLoad());
		DebugVisualizeCompositeMeshesAfterTonemapping = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_DebugVisualizeCompositeMeshes_AfterTonemapping.MI_Compositor_DebugVisualizeCompositeMeshes_AfterTonemapping")).TryLoad());

		DebugVisualizeShadowsMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/Materials/PostProcessing/MI_Compositor_DebugVisualizeShadows.MI_Compositor_DebugVisualizeShadows")).TryLoad());

		PostProcessSettings.AddBlendable(BeforeTranslucencyMaterial, 1.F);
		PostProcessSettings.AddBlendable(SsrInputMaterial, 1.F);
		PostProcessSettings.AddBlendable(AfterTonemappingMaterial, 1.F);
		PostProcessProperties.Settings = &PostProcessSettings;
	}

	virtual bool EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint) override
	{
		return PostProcessProperties.bIsEnabled;
	}

	virtual FPostProcessVolumeProperties GetProperties() const override
	{
		return PostProcessProperties;
	}

	void SetIsEnabled(bool NewIsEnabled)
	{
		PostProcessProperties.bIsEnabled = NewIsEnabled;
	}

	void SetDebugVisualizeCompositeMeshes(bool bNewDebugVisualizeCompositeMeshes)
	{
		if (bDebugVisualizeCompositeMeshes != bNewDebugVisualizeCompositeMeshes)
		{
			bDebugVisualizeCompositeMeshes = bNewDebugVisualizeCompositeMeshes;

			if (bDebugVisualizeCompositeMeshes)
			{
				PostProcessSettings.RemoveBlendable(DebugVisualizeCompositeMeshesBeforeTranslucencyMaterial);
				PostProcessSettings.RemoveBlendable(DebugVisualizeCompositeMeshesAfterTonemapping);

				PostProcessSettings.AddBlendable(DebugVisualizeCompositeMeshesBeforeTranslucencyMaterial, 1.F);
				PostProcessSettings.AddBlendable(DebugVisualizeCompositeMeshesAfterTonemapping, 1.F);

			}
			else
			{
				PostProcessSettings.RemoveBlendable(DebugVisualizeCompositeMeshesBeforeTranslucencyMaterial);
				PostProcessSettings.RemoveBlendable(DebugVisualizeCompositeMeshesAfterTonemapping);
			}
		}
	}

	void SetDebugVisualizeShadows(bool bNewDebugVisualizeShadows)
	{
		if (bDebugVisualizeShadows != bNewDebugVisualizeShadows)
		{
			bDebugVisualizeShadows = bNewDebugVisualizeShadows;

			if (bDebugVisualizeShadows)
			{
				PostProcessSettings.AddBlendable(DebugVisualizeShadowsMaterial, 1.F);

			}
			else
			{
				PostProcessSettings.RemoveBlendable(DebugVisualizeShadowsMaterial);
			}
		}
	}

#if DEBUG_POST_PROCESS_VOLUME_ENABLE
	virtual FString GetDebugName() const override
	{
		return FString("CompositorPostProcessVolume");
	}
#endif

private:
	FPostProcessSettings PostProcessSettings;
	FPostProcessVolumeProperties PostProcessProperties;

	UMaterialInterface* BeforeTranslucencyMaterial;
	UMaterialInterface* SsrInputMaterial;
	UMaterialInterface* AfterTonemappingMaterial;

	bool bDebugVisualizeCompositeMeshes;
	UMaterialInterface* DebugVisualizeCompositeMeshesBeforeTranslucencyMaterial;
	UMaterialInterface* DebugVisualizeCompositeMeshesAfterTonemapping;

	bool bDebugVisualizeShadows;
	UMaterialInterface* DebugVisualizeShadowsMaterial;
};