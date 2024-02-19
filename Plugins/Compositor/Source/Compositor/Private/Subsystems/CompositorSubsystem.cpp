// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/CompositorSubsystem.h"

#include "Engine/Engine.h" // GEngine

#include "CameraCalibrationSubsystem.h"
#include "Actors/CompositeMesh.h"
#include "Assets/Composite.h"
#include "Assets/CompositeKeyer.h"
#include "Components/SoftMaskCaptureComponent.h"
#include "Objects/CompositeWorldData.h"
#include "Objects/CompositeColorGrade.h"
#include "Objects/CompositeViewExtension.h"
#include "CompositeTypes.h"
#include "IDisplayCluster.h"

#include "GameFramework/GameModeBase.h"
#include "MoviePipelineGamemode.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MediaTexture.h"
// #include "LensDistortionComponent.h"
#include "Camera/CameraActor.h"
#include "Cluster/IDisplayClusterClusterManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameterCollection.h"
#include "Slate/SceneViewport.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "IAssetViewport.h"
#include "SLevelViewport.h"
#endif

DEFINE_LOG_CATEGORY(LogCompositor);

bool UCompositorSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{	
	const FModuleManager& ModuleManager = FModuleManager::Get();
	const bool bUsingNDisplay = ModuleManager.IsModuleLoaded(FName("DisplayCluster"));
	if (bUsingNDisplay)
	{		
		const EDisplayClusterNodeRole DisplayClusterNodeRole = IDisplayCluster::Get().GetClusterMgr()->GetClusterRole();
		
		// If nDisplay is running and the engine is part of the cluster we don't create the compositor subsystem.
		if (DisplayClusterNodeRole != EDisplayClusterNodeRole::None)
		{
			UE_LOG(LogCompositor, Log, TEXT("Compositor Subsystem is not created because it is part of an nDisplay Cluster."));
			return false;
		}
	}	
	
	return Super::ShouldCreateSubsystem(Outer);
}

void UCompositorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogCompositor, Log, TEXT("Initializing Compositor Subsystem"));
	
	MediaInputDefaultFallbackTexture = Cast<UTexture2D>(FSoftObjectPath(TEXT("/Compositor/MediaFramework/T_Compositor_MediaInputFallback.T_Compositor_MediaInputFallback")).TryLoad());
	PlanarReflectionTexture = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/CompositePlanarReflection/RT_CompositePlanarReflection.RT_CompositePlanarReflection")).TryLoad());

	MediaInputKeyedRenderTarget = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/CompositeKeyer/RT_MediaInputKeyed.RT_MediaInputKeyed")).TryLoad());
	UKismetRenderingLibrary::ClearRenderTarget2D(this, MediaInputKeyedRenderTarget, FLinearColor(0, 0, 0, 0));
	MediaInputCompositeKeyerDisabledFallbackMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeKeyer/M_CompositeKeyerDisabledFallback.M_CompositeKeyerDisabledFallback")).TryLoad());
	MediaInputCompositeKeyerDisabledFallbackMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, MediaInputCompositeKeyerDisabledFallbackMaterial, FName("MediaKeyerDisabledFallbackMID"), EMIDCreationFlags::Transient);
	
	DefaultUndistortTexture = Cast<UTexture2D>(FSoftObjectPath(TEXT("/Compositor/LensData/T_DefaultUndistort.T_DefaultUndistort")).TryLoad());
	MediaInputUndistortedTexture = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/LensData/RT_Compositor_MediaInputUndistorted.RT_Compositor_MediaInputUndistorted")).TryLoad());
	UKismetRenderingLibrary::ClearRenderTarget2D(this, MediaInputUndistortedTexture, FLinearColor(0, 0, 0, 0));
	MediaInputUndistortMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/LensData/M_MediaInputUndistort.M_MediaInputUndistort")).TryLoad());
	MediaInputUndistortMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, MediaInputUndistortMaterial, FName("MediaInputUndistortMID"), EMIDCreationFlags::Transient);

	CompositorMaterialParameterCollection = Cast<UMaterialParameterCollection>(FSoftObjectPath(TEXT("/Compositor/Materials/ParameterCollections/MPC_Compositor")).TryLoad());

	CompositeWorldData = GetCompositeWorldData();

	UWorld* World = GetWorld();
	check(IsValid(World))
	if (IsValid(World))
	{
		World->OnBeginPostProcessSettings.AddUObject(this, &UCompositorSubsystem::ComputeCompositePostProcess);
		World->InsertPostProcessVolume(&CompositePostProcessVolume);

		AWorldSettings* WorldSetting = GetWorld()->GetWorldSettings();
		if (IsValid(WorldSetting) && !SoftMaskCaptureComponent)
		{
			UE_LOG(LogCompositor, Log, TEXT("Registering soft mask component."));
			SoftMaskCaptureComponent = NewObject<USoftMaskCaptureComponent>(WorldSetting, NAME_None, RF_Transient);
			SoftMaskCaptureComponent->RegisterComponent();
		}

		World->OnWorldBeginPlay.RemoveAll(this);
		World->OnWorldBeginPlay.AddUObject(this, &UCompositorSubsystem::OnWorldBeginPlayCallback);

		// Create editor only preview camera.
#if WITH_EDITOR
		if (World->WorldType != EWorldType::Game && World->WorldType != EWorldType::PIE)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Name = "CompositePreviewCamera";
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;
			SpawnInfo.bTemporaryEditorActor = true;
			SpawnInfo.bHideFromSceneOutliner = true;
			CompositePreviewCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), SpawnInfo);
			TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
			CompositePreviewCamera->GetComponents(PrimitiveComponents);
			// Make sure all components including editor preview components are all hidden.
			for(UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
			{
				PrimitiveComponent->SetVisibility(false, true);
			}
			CompositePreviewCamera->SetActorLabel("Compositor Preview");
		}
#endif
	}

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UE_LOG(LogCompositor, Log, TEXT("Initializing Scene View Extention"));
		CompositeViewExtension = FSceneViewExtensions::NewExtension<FCompositeViewExtension>(this);
	}

	ClearReflectionCaptureRenderTarget();

	CompositeViewport = nullptr;
	
#if WITH_EDITOR
	bIsModifyViewportClientViewRegistered = false;
	CompositeLevelEditorViewportClient = nullptr;
#endif
}

void UCompositorSubsystem::Deinitialize()
{
	UE_LOG(LogCompositor, Log, TEXT("Deinitializing Compositor Subsystem"));

	if (SoftMaskCaptureComponent)
	{
		UE_LOG(LogCompositor, Log, TEXT("Unregistering soft mask component."));
		SoftMaskCaptureComponent->UnregisterComponent();
		SoftMaskCaptureComponent->DestroyComponent();
	}

	UWorld* World = GetWorld();
	check(IsValid(World));
	if (IsValid(World))
	{
		World->OnBeginPostProcessSettings.RemoveAll(this);
		World->RemovePostProcessVolume(&CompositePostProcessVolume);
		World->OnWorldBeginPlay.RemoveAll(this);
	}

	OnCompositeWorldDataAdded.Clear();
	OnCompositeWorldDataRemoved.Clear();
	OnCompositeUpdateInterfaceRegistered.Clear();
	OnCompositeUpdateInterfaceUnregistered.Clear();

#if WITH_EDITOR
	RegisterModifyViewportClientView(false);
	FEditorDelegates::EndPIE.RemoveAll(this);
#endif

	Super::Deinitialize();
}

void UCompositorSubsystem::OnWorldBeginPlayCallback()
{
	UE_LOG(LogCompositor, Log, TEXT("UCompositorSubsystem::OnWorldBeginPlayCallback"));
	if (CompositorMaterialParameterCollection)
	{
		AMoviePipelineGameMode* MoviePipelineGameMode = Cast<AMoviePipelineGameMode>(UGameplayStatics::GetGameMode(this));
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("IsMovieRenderQueueEnabled"), MoviePipelineGameMode ? 1.F : 0.F);
	}
}

void UCompositorSubsystem::UpdateLensData()
{
	if (GEngine)
	{		
		CurrentUndistortTexture = DefaultUndistortTexture;
		CameraFovWithoutOverscan = 90;
		CameraOverscanFactor = 1.F;
		
#if WITH_EDITORONLY_DATA
		UCameraComponent* EditorCameraComponent = nullptr;
		
		if (CompositeViewport && CompositeLevelEditorViewportClient)
		{
			EditorCameraComponent = CompositeLevelEditorViewportClient->GetCameraComponentForView();
		}
		
		if (GEditor && (GEditor->bIsSimulatingInEditor || !GEditor->IsPlayingSessionInEditor()) && (!GEditor->IsPlaySessionRequestQueued() || GEditor->IsSimulateInEditorQueued()))
		{
			if (IsValid(EditorCameraComponent))
			{
				CameraFovWithoutOverscan = EditorCameraComponent->FieldOfView;
			}
			else if (CompositeLevelEditorViewportClient)
			{
				CameraFovWithoutOverscan = CompositeLevelEditorViewportClient->FOVAngle;

				// FoV can sometimes be invalid in the editor when going between play modes, just make sure it is not lower than 5.
				if (CameraFovWithoutOverscan < 5.F)
				{
					CameraFovWithoutOverscan = 5.F;
				}
			}
		}
#endif

		const APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		if (IsValid(PlayerCameraManager))
		{
			CameraFovWithoutOverscan = PlayerCameraManager->GetFOVAngle();
		}
		
		UCameraCalibrationSubsystem* CameraCalibrationSubsystem = GEngine->GetEngineSubsystem<UCameraCalibrationSubsystem>();		
		if (IsValid(CameraCalibrationSubsystem))
		{
			const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (IsValid(PlayerController))
			{
				const AActor* ViewTarget = PlayerController->GetViewTarget();
				if (IsValid(ViewTarget))
				{
					UCineCameraComponent* CineCameraComponent = Cast<UCineCameraComponent>(ViewTarget->GetComponentByClass(UCineCameraComponent::StaticClass()));
					if (IsValid(CineCameraComponent))
					{
						FDistortionHandlerPicker DistortionHandlerPicker;
						DistortionHandlerPicker.TargetCameraComponent = CineCameraComponent;
						
					
						const ULensDistortionModelHandlerBase* DistortionModelHandlerBase = CameraCalibrationSubsystem->FindDistortionModelHandler(DistortionHandlerPicker);
						if (IsValid(DistortionModelHandlerBase))
						{
							// The lens distortion component does not have a public function to check if the distortion is applied (4.27) so we just check of the distort MID is on the camera.
							const TArray<FWeightedBlendable> WeightedBlendables = CineCameraComponent->PostProcessSettings.WeightedBlendables.Array;
							bool bIsDistortionApplied = false;
							for (int32 i = 0, count = WeightedBlendables.Num(); i < count; ++i)
							{
								if (WeightedBlendables[i].Object == DistortionModelHandlerBase->GetDistortionMID())
								{
									if (WeightedBlendables[i].Weight == 1.F)
									{
										bIsDistortionApplied = true;
										break;
									}
								}
							}
						
							if (bIsDistortionApplied)
							{
								CurrentUndistortTexture = DistortionModelHandlerBase->GetUndistortionDisplacementMap();
								CameraOverscanFactor = DistortionModelHandlerBase->GetOverscanFactor();
								float FocalLenth = CineCameraComponent->CurrentFocalLength;
								// Overrides the focal length with the original focal length in case it is adjusted for overscan.
								CameraCalibrationSubsystem->GetOriginalFocalLength(CineCameraComponent, FocalLenth);
								CameraFovWithoutOverscan = FMath::RadiansToDegrees(2.0f * FMath::Atan(CineCameraComponent->Filmback.SensorWidth / (2.0f * FocalLenth)));
							}							
						}
					}
				}
			}
		}
		

		if (CompositorMaterialParameterCollection)
		{
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, "CameraFovWithoutOverscan", CameraFovWithoutOverscan);
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, "CameraOverscanFactor", CameraOverscanFactor);
		}

		if (MediaInputUndistortMID)
		{
			MediaInputUndistortMID->SetTextureParameterValue("UndistortTexture", CurrentUndistortTexture);
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, MediaInputUndistortedTexture, MediaInputUndistortMID);
		}
	}
}

void UCompositorSubsystem::RegisterCompositeUpdateInterface(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface)
{
	UObject* InterfaceObject = CompositeUpdateInterface.GetObject();

	if (!IsValid(InterfaceObject))
	{
		return;
	}

	AActor* Actor = Cast<AActor>(InterfaceObject);
	// This check makes sure if the interface object is an Actor to not include preview actors to the list (drag/drop into the scene we create temporary actors).
	if (Actor && Actor->HasAnyFlags(RF_Transient))
	{
		return;
	}

	const int32 Index = CompositeUpdateInterfaceArray.AddUnique(CompositeUpdateInterface);
	if (Index >= 0)
	{
		// Always do an update for newly registered interfaces if a composite is already active.
		UComposite* WorldComposite = GetWorldComposite();
		if (IsValid(WorldComposite))
		{
			CompositeUpdateInterface->OnCompositeUpdate(*WorldComposite);
		}

		OnCompositeUpdateInterfaceRegistered.Broadcast(CompositeUpdateInterface);
	}
}

void UCompositorSubsystem::UnregisterCompositeUpdateInterface(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface)
{
	const int32 Index = CompositeUpdateInterfaceArray.Remove(CompositeUpdateInterface);
	if (Index >= 0)
	{
		OnCompositeUpdateInterfaceUnregistered.Broadcast(CompositeUpdateInterface);
	}
}

void UCompositorSubsystem::Tick(float DeltaTime)
{
	const UWorld* World = GetWorld();
	const UComposite* WorldComposite = GetWorldComposite();

	if (IsValid(World) && IsValid(CompositeWorldData) && IsValid(WorldComposite))
	{
		// Enforce new style output.
		if (UMediaTexture* MediaTexture = Cast<UMediaTexture>(WorldComposite->GetMediaInputTexture()))
		{
			if (!MediaTexture->NewStyleOutput)
			{
				MediaTexture->NewStyleOutput = true;
			}
		}		
		
		const bool bSetFixedViewportSize = IsValid(World) && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE) && CompositeWorldData->GetIsWorldCompositeEnabled();
		UpdateCompositeViewportInfo(bSetFixedViewportSize);
		
#if WITH_EDITOR
		if (!bIsModifyViewportClientViewRegistered && CompositeLevelEditorViewportClient)
		{
			RegisterModifyViewportClientView(true);
		}

		if (CompositePreviewCamera)
		{
			CompositePreviewCamera->GetCameraComponent()->SetConstraintAspectRatio(IsMediaTextureSizeValid());
			const FVector2D MediaTextureSize = GetMediaInputTextureSize();
			CompositePreviewCamera->GetCameraComponent()->SetAspectRatio(MediaTextureSize.X / MediaTextureSize.Y);
		}

#endif // WITH_EDITOR

		// Update the media input related render target's size so it matches the media input texture size. 

		const FIntPoint MediaTextureSize = GetMediaInputTextureSize();
		
		if (IsValid(MediaInputKeyedRenderTarget))
		{
			// Only update if the texture does not have the same size already.
			if (MediaTextureSize.X != MediaInputKeyedRenderTarget->SizeX || MediaTextureSize.Y != MediaInputKeyedRenderTarget->SizeY)
			{
				MediaInputKeyedRenderTarget->ResizeTarget(MediaTextureSize.X, MediaTextureSize.Y);
			}
		}
		if (IsValid(MediaInputUndistortedTexture))
		{
			// Only update if the texture does not have the same size already.
			if (MediaTextureSize.X != MediaInputUndistortedTexture->SizeX || MediaTextureSize.Y != MediaInputUndistortedTexture->SizeY)
			{
				MediaInputUndistortedTexture->ResizeTarget(MediaTextureSize.X, MediaTextureSize.Y);
			}
		}

		if (CompositorMaterialParameterCollection)
		{
			UCompositeKeyer* CompositeKeyer = WorldComposite->GetMediaInputKeyer();
			if (IsValid(CompositeKeyer) && CompositeKeyer->GetIsKeyerEnabled())
			{
				CompositeKeyer->UpdateCompositeKeyer(this);
				CompositeKeyer->GetCompositeKeyerMID()->SetTextureParameterValue("Compositor_MediaInputTexture", GetActiveMediaTexture());
				UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("IsKeyerEnabled"), true);
			}
			else
			{
				MediaInputCompositeKeyerDisabledFallbackMID->SetTextureParameterValue("Compositor_MediaInputTexture", GetActiveMediaTexture());
				UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, MediaInputKeyedRenderTarget, MediaInputCompositeKeyerDisabledFallbackMID);
				UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("IsKeyerEnabled"), false);
			}
			
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, "DebugMediaOverlay", CompositeWorldData->GetDebugMediaOverlay());

			const bool bUseDebugEditorCamera = CompositeWorldData->IsAllowedToUseDebugEditorCamera();
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, "MediaProjectionBlendAmount", bUseDebugEditorCamera);
			if (bUseDebugEditorCamera) // This bool already checked if the camera actors is valid.
			{
				const UCameraComponent* DebugCameraComponent = CompositeWorldData->GetDebugEditorCamera()->GetCameraComponent();
				UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, "MediaProjectionFoV", DebugCameraComponent->FieldOfView);
				UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection,
					"MediaProjectionCameraPosition",
					FLinearColor(DebugCameraComponent->GetComponentLocation()));
				UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection,
					"MediaProjectionCameraForward",
					FLinearColor(DebugCameraComponent->GetForwardVector()));
				UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection,
					"MediaProjectionCameraRight",
					FLinearColor(DebugCameraComponent->GetRightVector()));
				UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection,
					"MediaProjectionCameraUp",
					FLinearColor(DebugCameraComponent->GetUpVector()));
			}
		}
	}

	UpdateLensData();
}

// Make sure the tick function is only called for the subsystem
bool UCompositorSubsystem::IsTickable() const
{
	if (GetWorld())
	{
#if WITH_EDITOR
		if (GEditor)
		{
			FWorldContext* WorldContext = GEditor->GetPIEWorldContext();
			if (WorldContext)
			{
				return WorldContext->World() == GetWorld();
			}
			return GEditor->GetEditorWorldContext(false).World() == GetWorld();
		}
#endif
		return true;
	}
	return false;
}

ETickableTickType UCompositorSubsystem::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) || !GetWorld() || GetWorld()->IsNetMode(NM_DedicatedServer) ? ETickableTickType::Never : ETickableTickType::Conditional;
}

void UCompositorSubsystem::OnCompositeUpdate_Internal()
{
	UComposite* WorldComposite = GetWorldComposite();
	
	if (!IsValid(WorldComposite))
	{
		UE_LOG(LogCompositor, Error, TEXT("Can't do a composite update since the World Composite is null."));
		return;
	}
	
	for (TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface : CompositeUpdateInterfaceArray)
	{
		CompositeUpdateInterface->OnCompositeUpdate(*WorldComposite);
	}
}

void UCompositorSubsystem::ComputeCompositePostProcess(FVector ViewLocation, FSceneView* SceneView)
{
	const UComposite* WorldComposite = GetWorldComposite();
	
	if (IsValid(CompositeWorldData) && IsValid(WorldComposite) && CompositorMaterialParameterCollection)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("EnableSoftMask"), WorldComposite->GetEnableSoftMask());		
		const float ShadowsOffset = WorldComposite->GetShadowsOffset();
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("ShadowsBlackLevel"), WorldComposite->GetShadowsBlackLevel() + ShadowsOffset);
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("ShadowsWhiteLevel"), WorldComposite->GetShadowsWhiteLevel() + ShadowsOffset);
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("ShadowsGamma"), WorldComposite->GetShadowsGamma());
		UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ShadowsTint"), WorldComposite->GetShadowsTint());

		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("EnablePlanarReflection"), WorldComposite->GetEnablePlanarReflection());
		UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("PlanarReflectionColor"), WorldComposite->GetPlanarReflectionColor());
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("PlanarReflectionDistortionIntensity"), WorldComposite->GetPlanarReflectionDistortionIntensity());
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("PlanarReflectionDistortionOffset"), WorldComposite->GetPlanarReflectionDistortionOffset());

		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("MediaBlendNone"), WorldComposite->GetMediaBlend() == EMediaBlend::None ? 1.F : 0.F);
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("MediaBlendPreToneCurve"), WorldComposite->GetMediaBlend() == EMediaBlend::PreToneCurve ? 1.F : 0.F);
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("BrightnessMaskGamma"), WorldComposite->GetBrightnessMaskGamma());
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("ApplyInverseToneCurve"), WorldComposite->GetApplyInverseToneCurve() ? 1.F : 0.F);

		const EOutputAlpha OutputAlpha = WorldComposite->GetOutputAlpha();
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("OutputAlphaInvertedOpacity"), OutputAlpha == EOutputAlpha::InvertedOpacity ? 1.F : 0.F);
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("OutputAlphaWhite"), OutputAlpha == EOutputAlpha::White ? 1.F : 0.F);

		const bool bOutputAlphaOverride = (OutputAlpha == EOutputAlpha::White || OutputAlpha == EOutputAlpha::Black) && !CompositeWorldData->GetDebugVisualizeCompositeMeshes() && !CompositeWorldData->GetDebugVisualizeShadows();
		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("OutputAlphaOverride"), bOutputAlphaOverride ? 1.F : 0.F);

		UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("OutputRgbEncodingSrgb"), WorldComposite->GetOutputRgbEncoding() == EOutputRgbEncoding::Srgb ? 1.F : 0.F);

		UCompositeColorGrade* CompositeColorGrade = WorldComposite->GetCompositeColorGrade();
		if (CompositeColorGrade)
		{
			const FColorGradePerRangeSettings ColorGradeScene = CompositeColorGrade->GetColorGradeScene();
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeSceneSaturation"), FLinearColor(ColorGradeScene.Saturation));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeSceneContrast"), FLinearColor(ColorGradeScene.Contrast));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeSceneGamma"), FLinearColor(ColorGradeScene.Gamma));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeSceneGain"), FLinearColor(ColorGradeScene.Gain));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeSceneOffset"), FLinearColor(ColorGradeScene.Offset));
			
			const FColorGradePerRangeSettings ColorGradeMedia = CompositeColorGrade->GetColorGradeMedia();
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeMediaSaturation"), FLinearColor(ColorGradeMedia.Saturation));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeMediaContrast"), FLinearColor(ColorGradeMedia.Contrast));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeMediaGamma"), FLinearColor(ColorGradeMedia.Gamma));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeMediaGain"), FLinearColor(ColorGradeMedia.Gain));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeMediaOffset"), FLinearColor(ColorGradeMedia.Offset));
			
			const FColorGradePerRangeSettings ColorGradeCombined = CompositeColorGrade->GetColorGradeCombined();
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeCombinedSaturation"), FLinearColor(ColorGradeCombined.Saturation));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeCombinedContrast"), FLinearColor(ColorGradeCombined.Contrast));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeCombinedGamma"), FLinearColor(ColorGradeCombined.Gamma));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeCombinedGain"), FLinearColor(ColorGradeCombined.Gain));
			UKismetMaterialLibrary::SetVectorParameterValue(this, CompositorMaterialParameterCollection, FName("ColorGradeCombinedOffset"), FLinearColor(ColorGradeCombined.Offset));
		}				
	}

	bool bIsEnabled = false;
	
	if (IsValid(CompositeWorldData))
	{
		bIsEnabled = CompositeWorldData->GetIsWorldCompositeEnabled();
		//GetCompositePostProcessVolume().SetDebugVisualizeCompositeMeshes(CompositeWorldData->GetDebugVisualizeCompositeMeshes());
		//GetCompositePostProcessVolume().SetDebugVisualizeShadows(CompositeWorldData->GetDebugVisualizeShadows());
		CompositePostProcessVolume.SetDebugVisualizeCompositeMeshes(CompositeWorldData->GetDebugVisualizeCompositeMeshes());
		CompositePostProcessVolume.SetDebugVisualizeShadows(CompositeWorldData->GetDebugVisualizeShadows());

		if (CompositorMaterialParameterCollection)
		{
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("OutputAlphaInRgb"), CompositeWorldData->GetDebugVisualizeAlphaInRgb());
			UKismetMaterialLibrary::SetScalarParameterValue(this, CompositorMaterialParameterCollection, FName("DebugVisualizeCompositeMeshes"), CompositeWorldData->GetDebugVisualizeCompositeMeshes());
		}
	}

	// Compositing is always off for scene captures, at least, for now.
	// if we do allow compositing for scene captures we need to somehow filter the composite planar reflection.
	// Compositing is also disabled when the user is visualizing a buffer.
	if (SceneView->Family->EngineShowFlags.PostProcessing == 0 || SceneView->Family->ViewMode != EViewModeIndex::VMI_Lit || SceneView->bIsSceneCapture)
	{
		bIsEnabled = false;
	}

	CompositePostProcessVolume.SetIsEnabled(bIsEnabled);
}

void UCompositorSubsystem::ClearReflectionCaptureRenderTarget()
{
	if (PlanarReflectionTexture)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, PlanarReflectionTexture);
	}
}

#if WITH_EDITOR
void UCompositorSubsystem::RegisterModifyViewportClientView(bool bRegister)
{
	if (CompositeViewport && CompositeLevelEditorViewportClient)
	{
		// Always unregister.
		CompositeLevelEditorViewportClient->ViewModifiers.RemoveAll(this);

		bIsModifyViewportClientViewRegistered = false;

		if (bRegister)
		{
			CompositeLevelEditorViewportClient->ViewModifiers.AddUObject(this, &UCompositorSubsystem::ModifyViewportClientView);
			bIsModifyViewportClientViewRegistered = true;
		}
	}
}

// TODO: Check with devs why changing aspect ratio through ViewModifiers is not allowed, issue UE-121623 on JIRA.
void UCompositorSubsystem::ModifyViewportClientView(FEditorViewportViewModifierParams& EditorViewportViewModifierParams)
{
	const FIntPoint TextureSize = GetMediaInputTextureSize();
	EditorViewportViewModifierParams.ViewInfo.bConstrainAspectRatio = true;
	EditorViewportViewModifierParams.ViewInfo.AspectRatio = static_cast<float>(TextureSize.X) / static_cast<float>(TextureSize.Y);
}
#endif // WITH_EDITOR

UCompositeWorldData* UCompositorSubsystem::FindOrAddCompositeWorldData()
{
	if (!GetCompositeWorldData()) //GetCompositeWorldData stores the result in CompositeWorldData.
	{
		AWorldSettings* WorldSetting = GetWorld()->GetWorldSettings();
		if (IsValid(WorldSetting))
		{
			CompositeWorldData = NewObject<UCompositeWorldData>(WorldSetting, FName("CompositeWorldData"));
			WorldSetting->AddAssetUserData(CompositeWorldData);
			OnCompositeUpdate_Internal();
			OnCompositeWorldDataAddedRaw.Broadcast();
			OnCompositeWorldDataAdded.Broadcast();
		}
	}

	return CompositeWorldData;
}

UCompositeWorldData* UCompositorSubsystem::GetCompositeWorldData()
{
	if (CompositeWorldData == nullptr)
	{
		AWorldSettings* WorldSetting = GetWorld()->GetWorldSettings();
		if (IsValid(WorldSetting))
		{
			CompositeWorldData = CastChecked<UCompositeWorldData>(WorldSetting->GetAssetUserDataOfClass(UCompositeWorldData::StaticClass()), ECastCheckedType::NullAllowed);
		}
	}

	return CompositeWorldData;
}

void UCompositorSubsystem::RemoveCompositeWorldData()
{
	AWorldSettings* WorldSetting = GetWorld()->GetWorldSettings();
	if (IsValid(WorldSetting))
	{
		// Only remove when it is there so we only fire OnCompositeWorldDataRemoved when we remove the asset user data.
		if (WorldSetting->GetAssetUserDataOfClass(UCompositeWorldData::StaticClass()))
		{
			WorldSetting->RemoveUserDataOfClass(UCompositeWorldData::StaticClass());
			CompositeWorldData = nullptr;
			OnCompositeWorldDataRemovedRaw.Broadcast();
			OnCompositeWorldDataRemoved.Broadcast();
		}
	}
}

bool UCompositorSubsystem::IsMediaTextureValid() const
{
	return IsValid(GetWorldComposite()) && IsValid(GetWorldComposite()->GetMediaInputTexture());
}

USoftMaskCaptureComponent* UCompositorSubsystem::GetSoftMaskCaptureComponent() const
{
	if (IsValid(SoftMaskCaptureComponent))
	{
		return SoftMaskCaptureComponent;
	}

	return nullptr;
}

void UCompositorSubsystem::UpdateCompositeViewportInfo(bool bSetFixedSize)
{
// 	// OLD STUFF
//
// 	// It is important to check the active world first, even in editor, just so play mode works as expected.
// 	const UWorld* World = GetWorld();
// 	if (IsValid(World)
// #if WITH_EDITOR
// 	&& GEditor && !GEditor->IsSimulateInEditorInProgress() // Skip this if we are in simulation mode.
// #endif
// 	)
// 	{
// 		if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
// 		{
// 			const UGameViewportClient* GameViewportClient = World->GetGameViewport();
// 			if (IsValid(GameViewportClient))
// 			{
// 				CompositeViewport = GameViewportClient->Viewport;
// 				return;
// 			}
// 		}
// 	}
//
// #if WITH_EDITOR
// 	if (GEditor)
// 	{
// 		for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
// 		{
// 			if (LevelVC && LevelVC->IsPerspective())
// 			{
// 				CompositeLevelEditorViewportClient = LevelVC;
// 				CompositeViewport = CompositeLevelEditorViewportClient->Viewport;
// 				break;
// 			}
// 		}
// 	}
// #endif


	//NEW STUFF
	
	FVector2D DesiredSize;	
	TSharedPtr<FSceneViewport> SceneViewport;

#if WITH_EDITOR
	// Is it a "standalone" window
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::PIE)
		{
			UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
			FSlatePlayInEditorInfo& Info = EditorEngine->SlatePlayInEditorMap.FindChecked(Context.ContextHandle);
			if (Info.SlatePlayInEditorWindowViewport.IsValid())
			{
				SceneViewport = Info.SlatePlayInEditorWindowViewport;
			}
		}
	}

	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	if (!SceneViewport.IsValid())
	{
		// Find a editor viewport
		TSharedPtr<IAssetViewport> ViewportInterface = LevelEditorModule.GetFirstActiveViewport();
		if (ViewportInterface.IsValid())
		{
			SceneViewport = ViewportInterface->GetSharedActiveViewport();
		}
	}
#endif

	if (SceneViewport.IsValid())
	{
		const FIntPoint MediaInputTextureSize = GetMediaInputTextureSize();		
		if (bSetFixedSize && IsMediaTextureSizeValid() && GetCompositeWorldData() && GetCompositeWorldData()->GetMatchViewportResolutionWithMediaInput())
		{	
			if (MediaInputTextureSize != SceneViewport->GetRenderTargetTextureSizeXY())
			{
				SceneViewport->SetFixedViewportSize(MediaInputTextureSize.X, MediaInputTextureSize.Y);
			}
		}
		else // If viewport should not be fixed.
		{
			// If the viewport currently has a fixed size.
			if (SceneViewport->HasFixedSize())
			{
				SceneViewport->SetFixedViewportSize(0,0);
			}
		}
	}
	
	CompositeViewport = SceneViewport.Get();

	
// 	// It is important to check the active world first, even in editor, just so play mode works as expected.
// 	const UWorld* World = GetWorld();
// 	if (IsValid(World)
// #if WITH_EDITOR
// 	&& GEditor && !GEditor->IsSimulateInEditorInProgress() // Skip this if we are in simulation mode.
// #endif
// 	)
// 	{
// 		if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
// 		{
// 			const UGameViewportClient* GameViewportClient = World->GetGameViewport();
// 			if (IsValid(GameViewportClient))
// 			{
// 				CompositeViewport = GameViewportClient->Viewport;
//
// 				if (CompositeViewport)
// 				{
// 					return CompositeViewport;
// 				}
// 			}
// 		}
// 	}

#if WITH_EDITOR
	const UWorld* World = GetWorld();
	if (World)
	{
		if (GEditor && !(World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE))
		{
			for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
			{
				if (LevelVC && LevelVC->IsPerspective())
				{
					CompositeLevelEditorViewportClient = LevelVC;
					CompositeViewport = CompositeLevelEditorViewportClient->Viewport;
					break;
				}
			}
		}

		if (CompositeWorldData && CompositeWorldData->GetAutoPilotEditorPreviewCamera())
		{
			SLevelViewport* LevelViewport = LevelEditorModule.GetFirstActiveLevelViewport().Get();
			if (LevelViewport && LevelViewport->GetActiveViewport() == CompositeViewport)
			{
				if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
				{
					LevelViewport->GetLevelViewportClient().SetActorLock(nullptr);
				}
				else
				{
					if (LevelViewport->IsAnyActorLocked())
					{
						if (LevelViewport->IsActorLocked(CompositePreviewCamera))
						{
							LevelViewport->GetLevelViewportClient().ViewTransformPerspective.SetLocation(CompositePreviewCamera->GetActorLocation());
							LevelViewport->GetLevelViewportClient().ViewTransformPerspective.SetRotation(CompositePreviewCamera->GetActorRotation());
							LevelViewport->GetLevelViewportClient().bLockedCameraView = true;
						}						
					}
					else
					{
						if (CompositePreviewCamera)
						{
							const FViewportCameraTransform ViewTransform = LevelViewport->GetLevelViewportClient().ViewTransformPerspective;
							CompositePreviewCamera->SetActorLocationAndRotation(ViewTransform.GetLocation(), ViewTransform.GetRotation());
						}
						LevelViewport->OnActorLockToggleFromMenu(CompositePreviewCamera);
					}
				}
			}				
		}

		// if (GEditor && GEditor->IsPlaySessionInProgress())
		// {
		// 	if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
		// 	{
		// 		const UGameViewportClient* GameViewportClient = World->GetGameViewport();
		// 		if (IsValid(GameViewportClient))
		// 		{
		// 			CompositeViewport = GameViewportClient->Viewport;
		// 		}
		// 	}
		// }
	}
#endif
}

UTexture* UCompositorSubsystem::GetActiveMediaTexture() const
{
	if (CompositeWorldData)
	{
		if (const UComposite* WorldComposite = CompositeWorldData->GetWorldComposite())
		{
			if (UTexture* MediaTexture = WorldComposite->GetMediaInputTexture())
			{
				return MediaTexture;
			}
		}
	}
	return MediaInputDefaultFallbackTexture;
}

UComposite* UCompositorSubsystem::GetWorldComposite() const
{
	if (IsValid(CompositeWorldData))
	{
		return CompositeWorldData->GetWorldComposite();
	}

	return nullptr;
}

FIntPoint UCompositorSubsystem::GetMediaInputTextureSize() const
{	
	if (const UTexture* Texture = GetActiveMediaTexture())
	{
		return FIntPoint(FMath::Max(Texture->GetSurfaceWidth(), 2),FMath::Max(Texture->GetSurfaceHeight(), 2));
	}

	return FIntPoint(1920, 1080);
}

FIntPoint UCompositorSubsystem::GetViewportSize() const
{
	if (CompositeViewport)
	{
		return  CompositeViewport->GetRenderTargetTextureSizeXY();
	}
	return GetMediaInputTextureSize();
}
