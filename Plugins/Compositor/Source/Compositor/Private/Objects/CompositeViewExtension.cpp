// Copyright Epic Games, Inc. All Rights Reserved.

#include "Objects/CompositeViewExtension.h"

#include "Subsystems/CompositorSubsystem.h"
#include "Objects/CompositeWorldData.h"

#include "Materials/MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"

#include "SceneView.h"
#include "Camera/CameraActor.h"

//------------------------------------------------------------------------------
FCompositeViewExtension::FCompositeViewExtension(const FAutoRegister& AutoRegister, UCompositorSubsystem* Owner)
	: FSceneViewExtensionBase(AutoRegister)
	, CompositorSubsystem(Owner)
{}

void FCompositeViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{	
	// Don't do anything for scene captures or orthographic views.
	if (InView.bIsSceneCapture || !InView.IsPerspectiveProjection())
		return;
		
	if (CompositorSubsystem.IsValid())
	{
		//InView.CameraConstrainedViewRect

		// if(InView.ViewLocation == CompositorSubsystem->GetCompositePreviewCamera()->GetActorLocation())
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("View Actor is Compositor Preview Camera") );
		// 	CompositorSubsystem->GetCompositePostProcessVolume().SetDebugVisualizeCompositeMeshes(true);
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("View Actor is not Compositor Preview Camera") );
		// 	CompositorSubsystem->GetCompositePostProcessVolume().SetDebugVisualizeCompositeMeshes(false);			
		// }
		
		UMaterialParameterCollection* CompositorMaterialParameterCollection = CompositorSubsystem->GetCompositorMaterialParameterCollection();
		if (IsValid(CompositorMaterialParameterCollection))
		{
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("FilmBlackClip"), InView.FinalPostProcessSettings.FilmBlackClip);
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("FilmWhiteClip"), InView.FinalPostProcessSettings.FilmWhiteClip);
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("FilmShoulder"), InView.FinalPostProcessSettings.FilmShoulder);
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("FilmSlope"), InView.FinalPostProcessSettings.FilmSlope);
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("FilmToe"), InView.FinalPostProcessSettings.FilmToe);

			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("BlueCorrection"), InView.FinalPostProcessSettings.BlueCorrection);
			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("ToneCurveAmount"), InView.FinalPostProcessSettings.ToneCurveAmount);

			UKismetMaterialLibrary::SetScalarParameterValue(CompositorSubsystem.Get(), CompositorMaterialParameterCollection, FName("ToneCurveAmount"), 1);
		}

		UCompositeWorldData* CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
		if (CompositeWorldData)
		{
			InView.bCameraMotionBlur = CompositeWorldData->GetEnableCameraMotionBlur();
		}
	}

	InViewFamily.EngineShowFlags.SetToneCurve(false);
	InView.FinalPostProcessSettings.bOverride_ToneCurveAmount = true;
	InView.FinalPostProcessSettings.ToneCurveAmount = 0.F;
	InView.FinalPostProcessSettings.bOverride_BlueCorrection = true;
	InView.FinalPostProcessSettings.BlueCorrection = 0.F;

	InViewFamily.SceneCaptureSource = SCS_FinalColorHDR;
}

int32 FCompositeViewExtension::GetPriority() const
{
	return 50;
}

//------------------------------------------------------------------------------
bool FCompositeViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
// #if WITH_EDITOR
// 	// Disable the compositing if the editor is in play mode but the context we receive is not in game.
// 	if (GEditor && GEditor->IsPlaySessionInProgress() && (Context.GetWorld()->WorldType != EWorldType::Game && Context.GetWorld()->WorldType != EWorldType::PIE))
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Play session in progress.") );
// 		return  false;
// 	}
// #endif
	
	bool bActive = false;
	if (CompositorSubsystem.IsValid())
	{
		if (CompositorSubsystem->GetCompositeViewport() == Context.Viewport)
		{
			const UCompositeWorldData* CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
			if (IsValid(CompositeWorldData))
			{
				bActive = CompositeWorldData->GetIsWorldCompositeEnabled();
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s - Composite View Extension Active = %s"), *Context.GetWorld()->GetPathName(), bActive ? *FString("True") : *FString("False") );
	
	return bActive;
}

