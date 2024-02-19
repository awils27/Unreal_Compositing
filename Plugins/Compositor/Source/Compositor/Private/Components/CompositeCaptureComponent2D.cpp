// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/CompositeCaptureComponent2D.h"

#include "Camera/CameraActor.h"
#include "Subsystems/CompositorSubsystem.h"

#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h" 
#include "Objects/CompositeWorldData.h"

UCompositeCaptureComponent2D::UCompositeCaptureComponent2D()
{
	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;
	bCaptureOnMovement = false;
	ShowFlags.AntiAliasing = false;
	ShowFlags.Atmosphere = false;
	ShowFlags.BSP = false;
	ShowFlags.Decals = false;
	ShowFlags.Fog = false;
	ShowFlags.Particles = false;
	ShowFlags.DeferredLighting = false;
	ShowFlags.InstancedFoliage = false;
	ShowFlags.InstancedGrass = false;
	ShowFlags.Paper2DSprites = false;
	ShowFlags.TemporalAA = false;
	ShowFlags.Bloom = false;
	ShowFlags.EyeAdaptation = false;
	ShowFlags.MotionBlur = false;
	ShowFlags.ToneCurve = false;
	ShowFlags.SkyLighting = false;
	ShowFlags.AmbientOcclusion = false;
	ShowFlags.DynamicShadows = false;
	ShowFlags.AmbientCubemap = false;
	ShowFlags.DistanceFieldAO = false;
	ShowFlags.LightFunctions = false;
	ShowFlags.LightShafts = false;
	ShowFlags.ReflectionEnvironment = false;
	ShowFlags.ScreenSpaceReflections = false;
	ShowFlags.TexturedLightProfiles = false;
	ShowFlags.VolumetricFog = false;
	ShowFlags.Game = false;
	ShowFlags.Lighting = false;
	ShowFlags.PostProcessing = false;
}

void UCompositeCaptureComponent2D::OnRegister()
{
	Super::OnRegister();

	UKismetRenderingLibrary::ClearRenderTarget2D(this, TextureTarget, FLinearColor(0, 0, 0, 0));

	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
		if (IsValid(CompositorSubsystem))
		{						
			CompositorSubsystem->OnCompositeUpdateInterfaceRegistered.AddUObject(this, &UCompositeCaptureComponent2D::OnCompositeUpdateInterfaceRegistered);
			CompositorSubsystem->OnCompositeUpdateInterfaceUnregistered.AddUObject(this, &UCompositeCaptureComponent2D::OnCompositeUpdateInterfaceUnregistered);
			
			// Can fail on editor start up or during edit time, which is why we also try to get it during tick (IF WITH_EDITOR).
			CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
			WorldComposite = CompositorSubsystem->GetWorldComposite();
			//TryAddCompositeViewportResizeEvent();
		}
	}

	// Disable camera mesh because we are moving these components around in the editor and we don't want this mesh to ever obstruct the users view.
#if WITH_EDITORONLY_DATA
	if (ProxyMeshComponent)
	{
		ProxyMeshComponent->SetVisibility(false);
	}
#endif
}

void UCompositeCaptureComponent2D::OnUnregister()
{
	Super::OnUnregister();

	// if (CompositorSubsystem)
	// {
	// 	FViewport* CompositeViewport = CompositorSubsystem->GetCompositeViewport();
	// 	if (CompositeViewport)
	// 	{
	// 		CompositeViewport->ViewportResizedEvent.RemoveAll(this);
	// 	}		
	// }	
}

void UCompositeCaptureComponent2D::BeginPlay()
{
	Super::BeginPlay();

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

float UCompositeCaptureComponent2D::GetTargetTextureScreenPercentage() const
{
	return 100.F;
}

bool UCompositeCaptureComponent2D::GetIsCompositeCaptureActive() const
{
	return true;
}

void UCompositeCaptureComponent2D::UpdateRenderTargetSize()
{
	if (CompositorSubsystem)
	{
		const FIntPoint MediaTextureSize = CompositorSubsystem->GetMediaInputTextureSize();
		const float NormalizedScreenPercentage = (GetTargetTextureScreenPercentage() / 100.F);
		const uint32 NewSizeX = FMath::Max(static_cast<float>(MediaTextureSize.X) * NormalizedScreenPercentage, 2.F);
		const uint32 NewSizeY = FMath::Max(static_cast<float>(MediaTextureSize.Y) * NormalizedScreenPercentage, 2.F);

		if (TextureTarget->SizeX != NewSizeX || TextureTarget->SizeY != NewSizeY)
		{
			TextureTarget->ResizeTarget(NewSizeX, NewSizeY);
		}
	}	
}

void UCompositeCaptureComponent2D::UpdateCameraData(FVector& OutLocation, FRotator& OutRotation, float& OutFieldOfView, FVector& OutClipPlaneBase, FVector& OutClipBaseNormal) const
{
	// Do debug camera projection.
	if (bAllowDebugEditorCamera && CompositeWorldData && CompositeWorldData->IsAllowedToUseDebugEditorCamera())
	{
		const UCameraComponent* DebugCameraComponent = CompositeWorldData->GetDebugEditorCamera()->GetCameraComponent();
		OutLocation = DebugCameraComponent->GetComponentLocation();
		OutRotation = DebugCameraComponent->GetComponentRotation();
		OutFieldOfView = DebugCameraComponent->FieldOfView;
		
		return;
	}
	
	
#if WITH_EDITORONLY_DATA
	if (GEditor && (GEditor->bIsSimulatingInEditor || !GEditor->IsPlayingSessionInEditor()) && (!GEditor->IsPlaySessionRequestQueued() || GEditor->IsSimulateInEditorQueued()))
	{
		if (IsValid(EditorCameraComponent))
		{
			const FTransform CameraTransform = EditorCameraComponent->GetComponentTransform();
			OutLocation = CameraTransform.GetLocation();
			OutRotation = CameraTransform.GetRotation().Rotator();
			OutFieldOfView = EditorCameraComponent->FieldOfView;
		}
		else if (LevelEditorViewportClient)
		{
			if (LevelEditorViewportClient->bUsingOrbitCamera)
			{
				FMatrix OrbitMatrix = LevelEditorViewportClient->ViewTransformPerspective.ComputeOrbitMatrix();
				OrbitMatrix = OrbitMatrix.InverseFast();

				OutRotation = OrbitMatrix.Rotator();
				OutLocation = OrbitMatrix.GetOrigin();
			}
			else
			{
				OutLocation = LevelEditorViewportClient->ViewTransformPerspective.GetLocation();
				OutRotation = LevelEditorViewportClient->ViewTransformPerspective.GetRotation();
			}

			OutFieldOfView = LevelEditorViewportClient->FOVAngle;

			// FoV can sometimes be invalid in the editor when going between play modes, just make sure it is not lower than 5.
			if (OutFieldOfView < 5.F)
			{
				OutFieldOfView = 5.F;
			}
		}
		// Always return when editor data should set the transform.
		return;
	}
#endif

	if (IsValid(PlayerCameraManager))
	{
		OutLocation = PlayerCameraManager->GetCameraLocation();
		OutRotation = PlayerCameraManager->GetCameraRotation();
		OutFieldOfView = PlayerCameraManager->GetFOVAngle();
	}
}

// void UCompositeCaptureComponent2D::ViewportResizeCallback(FViewport* Viewport, uint32 Unused)
// {
// 	// We have to check if the incoming viewport is the same as the composite viewport because Unreal will call this function fro every viewport.
// 	if (!Viewport || !CompositorSubsystem || Viewport != CompositorSubsystem->GetCompositeViewport() || !TextureTarget || !CompositorSubsystem || !WorldComposite)
// 	{
// 		return;
// 	}
//
// 	const FIntPoint ViewportSize = Viewport->GetRenderTargetTextureSizeXY();
//
// 	if (GetIsCompositeCaptureActive())
// 	{
// 		if (ViewportSize.X > 2 && ViewportSize.Y > 2)
// 		{
// 			const float NormalizedScreenPercentage = (GetTargetTextureScreenPercentage() / 100.F);
// 			const FIntPoint NewTargetTextureSize = FIntPoint(FMath::Max(static_cast<float>(ViewportSize.X) * NormalizedScreenPercentage, 2.F), FMath::Max(static_cast<float>(ViewportSize.Y) * NormalizedScreenPercentage, 2.F));
// 			const FIntPoint CurrentTargetTextureSize = FIntPoint(TextureTarget->SizeX, TextureTarget->SizeY);
//
// 			if (CurrentTargetTextureSize != NewTargetTextureSize)
// 			{
// 				TextureTarget->ResizeTarget(NewTargetTextureSize.X, NewTargetTextureSize.Y);
//
// 				// Force viewport capture on resize to make sure the render targets respond properly. Disabling bCaptureEveryFrame just makes sure we don't get spammed with warnings about capturing twice.
// 				// This is however expensive to do since the editor viewport is not really smart with updating its size, it will call the resize function multiple times.
// 				const bool bStoredCaptureEveryFrame = bCaptureEveryFrame;
// 				bCaptureEveryFrame = false;
// 				CaptureScene();
// 				bCaptureEveryFrame = bStoredCaptureEveryFrame;
// 			}
// 		}
// 	}
// }

void UCompositeCaptureComponent2D::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetIsCompositeCaptureActive())
	{
		bCaptureEveryFrame = true;
	}
	else if (bCaptureEveryFrame)
	{
		bCaptureEveryFrame = false;
	}

	if (CompositorSubsystem)
	{
		FViewport* CompositeViewport = CompositorSubsystem->GetCompositeViewport();

		UpdateRenderTargetSize();
		
		// if (CompositeViewport)
		// {
		// 	if(PreviousScreenPercentage != GetTargetTextureScreenPercentage())
		// 	{
		// 		//ViewportResizeCallback(CompositeViewport, 0);
		// 		PreviousScreenPercentage = GetTargetTextureScreenPercentage();
		// 	}
		//
		// 	// Call viewport resize to update the target texture if it is out of sync.
		// 	if (GetIsCompositeCaptureActive())
		// 	{
		// 		if (TextureTarget && TextureTarget->SizeX <= 2)
		// 		{
		// 			//ViewportResizeCallback(CompositeViewport, 0);
		// 		}
		// 	}
		// }

#if WITH_EDITOR
		// This can not be stored at the start of the editor because it is invalid. Just store everything again when anything becomes invalid.
		if (!CompositeViewport || !CompositeWorldData || !WorldComposite)
		{
			CompositeViewport = CompositorSubsystem->GetCompositeViewport();
			CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
			WorldComposite = CompositorSubsystem->GetWorldComposite();
		}	
		
		if (CompositeViewport)
		{
			LevelEditorViewportClient = static_cast<FLevelEditorViewportClient*>(CompositeViewport->GetClient());
			if (LevelEditorViewportClient)
			{
				EditorCameraComponent = LevelEditorViewportClient->GetCameraComponentForView();
			}
		}
#endif
	}
}

void UCompositeCaptureComponent2D::UpdateSceneCaptureContents(FSceneInterface* Scene)
{
	FVector CameraLocation = FVector::ZeroVector;
	FRotator CameraRotation = FRotator::ZeroRotator;
	float CameraFieldOfView = 90.F;
	FVector CameraClipPlaneBase = FVector::ZeroVector;
	FVector CameraClipPlaneNormal = FVector::UpVector;
	
	UpdateCameraData(CameraLocation, CameraRotation, CameraFieldOfView, CameraClipPlaneBase, CameraClipPlaneNormal);

	SetWorldLocationAndRotation(CameraLocation, CameraRotation);
	FOVAngle = CameraFieldOfView;
	ClipPlaneBase = CameraClipPlaneBase;
	ClipPlaneNormal = CameraClipPlaneNormal;

	Super::UpdateSceneCaptureContents(Scene);
}
