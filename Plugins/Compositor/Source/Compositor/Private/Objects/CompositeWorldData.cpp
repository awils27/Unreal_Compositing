// Copyright Epic Games, Inc. All Rights Reserved.

#include "Objects/CompositeWorldData.h"

#include "Subsystems/CompositorSubsystem.h"
#include "Assets/Composite.h"
#include "Actors/CompositePlanarReflection.h"
#include "Camera/CameraActor.h"
#include "Components/CompositePlanarReflectionComponent.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Subsystems/CompositorSubsystem.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "LevelEditorViewport.h"
#endif

UCompositeWorldData::UCompositeWorldData()
{
    bIsCompositingEnabled = true;
    bAutoPilotEditorPreviewCamera = true;
    bMatchViewportResolutionWithMediaInput = true;
    bEnableCameraMotionBlur = false; // Disable camera motion blur by defaults due to artifacts it can cause, especially when keying.
	WorldComposite = CreateDefaultSubobject<UComposite>(TEXT("Composite"), /* bTransient = */false);
}

ACompositePlanarReflection* UCompositeWorldData::GetCompositePlanarReflection() const
{
    return CompositePlanarReflection;
}

void UCompositeWorldData::SetCompositePlanarReflection(ACompositePlanarReflection* NewCompositePlanarReflection)
{
    if (CompositePlanarReflection)
    {
        CompositePlanarReflection->GetCaptureComponent2D()->bCaptureEveryFrame = false;
        UKismetRenderingLibrary::ClearRenderTarget2D(this, CompositePlanarReflection->GetCaptureComponent2D()->TextureTarget, FLinearColor(0, 0, 0, 0));
    }

    CompositePlanarReflection = NewCompositePlanarReflection;

    if (CompositePlanarReflection)
    {
        CompositePlanarReflection->GetCaptureComponent2D()->bCaptureEveryFrame = true;
    }
}

bool UCompositeWorldData::IsAllowedToUseDebugEditorCamera() const
{
    const UWorld* World = GetWorld();
    if (World && IsValid(DebugEditorCamera))
    {
        return bUseDebugEditorCamera && (World->WorldType == EWorldType::Editor || bUseDebugEditorCameraInPlayMode);
    }
    return false;
}

#if WITH_EDITOR
void UCompositeWorldData::PreEditChange(FProperty* PropertyAboutToChange)
{
    const FName PropertyName = (PropertyAboutToChange != nullptr) ? PropertyAboutToChange->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, CompositePlanarReflection))
    {
        // Disable rendering on the current planar reflection before the new one is set.
        SetCompositePlanarReflection(nullptr);
    }

    Super::PreEditChange(PropertyAboutToChange);
}

void UCompositeWorldData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, CompositePlanarReflection))
    {
        SetCompositePlanarReflection(CompositePlanarReflection);
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, bDebugVisualizeCompositeMeshes))
    {
        bDebugVisualizeShadows = false;
        bDebugVisualizeAlphaInRgb = false;
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, bDebugVisualizeShadows))
    {
        bDebugVisualizeCompositeMeshes = false;
        bDebugVisualizeAlphaInRgb = false;
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, bDebugVisualizeAlphaInRgb))
    {
        bDebugVisualizeCompositeMeshes = false;
        bDebugVisualizeShadows = false;
    }

// #if WITH_EDITOR
//     if (PropertyName == GET_MEMBER_NAME_CHECKED(UCompositeWorldData, bAutoPilotEditorPreviewCamera))
//     {
//         const UWorld* World = GetWorld();
//         if (World)
//         {
//             UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
//             if (CompositorSubsystem && GEditor && !(World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE))
//             {
//                 for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
//                 {
//                     if (LevelVC && LevelVC->IsPerspective())
//                     {
//                     
//                         if (bAutoPilotEditorPreviewCamera)
//                         {
//                             LevelVC->viewpo
//                         }
//                     }
//                 }
//             }
//             // FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
//             //       SLevelViewport* LevelViewport = LevelEditorModule.GetFirstActiveLevelViewport().Get();
//             //       if (LevelViewport)
//             //       {
//             //           if (bAutoPilotEditorPreviewCamera)
//             //           {
//             //               UCompositorSubsystem* CompositorSubsystem = GetWorld()->GetSubsystem<UCompositorSubsystem>();
//             //               if (CompositorSubsystem)
//             //               {
//             //                   LevelViewport->OnActorLockToggleFromMenu(CompositorSubsystem->GetCompositePreviewCamera());
//             //               }
//             //           }
//             //           else
//             //           {
//             //               LevelViewport->GetLevelViewportClient().SetActorLock(nullptr);
//             //           }
//             //       }
//         }
//     }
// #endif
}
#endif
