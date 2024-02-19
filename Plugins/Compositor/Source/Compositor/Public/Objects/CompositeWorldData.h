// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "CompositeWorldData.generated.h"

class UComposite;
class ACompositePlanarReflection;
class ACameraActor;

/**
 * Asset user data added to the world settings which keeps track of the Composite used in the world.
 */
UCLASS()
class COMPOSITOR_API UCompositeWorldData : public UAssetUserData
{
	GENERATED_BODY()
	
public:
	UCompositeWorldData();

private:
	/** Is compositing active for the world. */
	UPROPERTY(Category = "CompositeWorldData", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsCompositingEnabled;
    
	/** Visualize the composite meshes by projecting a world aligned grid. */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, Transient, meta = (AllowPrivateAccess = "true"))
	bool bDebugVisualizeCompositeMeshes;

	/** Output the shadows to the RGB for debugging. */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, Transient, meta = (AllowPrivateAccess = "true"))
	bool bDebugVisualizeShadows;

	/** Output alpha in rgb for debugging. */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, Transient, meta = (AllowPrivateAccess = "true"))
	bool bDebugVisualizeAlphaInRgb;

	/** Debugging value for fading out the scene. */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, Transient, meta = (UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1, AllowPrivateAccess = "true"))
	float DebugMediaOverlay;

	/** Is the editor debug camera currently in use? */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bUseDebugEditorCamera;

	/** Is the editor debug camera allowed during play mode? This is not recommended due to it jittering. */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "bUseDebugEditorCamera"))
	bool bUseDebugEditorCameraInPlayMode;
	
	/**
	 * The camera being used to project the media input from during editor time.
	 * The image will vibrate a little bit due to it being projected after temporal anti-aliasing.
	 * This is also the reason why it is not supported during play mode.
	 */
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "bUseDebugEditorCamera"))
	ACameraActor* DebugEditorCamera;
	
	/** Automatically pilot of the Compositor Preview camera during editor time. */
	UPROPERTY(Category = "Viewport", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bAutoPilotEditorPreviewCamera;

	/** Automatically resize the viewport to the media texture input at runtime. */
	UPROPERTY(Category = "Viewport", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bMatchViewportResolutionWithMediaInput;
    
	/**
	* Is camera motion blur enabled in the scene.
	* This is disabled by default to minimize artifacts on the composite mask in the scene.
	* Only enable this when you are not keying.
	* This only disables motion blur caused by the movement of the camera, object based motion blur will still work when this is disabled.
	*/
	UPROPERTY(Category = "SceneView", EditAnywhere, BlueprintReadWrite, Transient, meta = (AllowPrivateAccess = "true"))
	bool bEnableCameraMotionBlur;
	
	/** The composite stored for this world, this is instanced so the user can adjust the variables easily per world while still able to have global settings using the parent asset. */
	UPROPERTY(Category = "CompositeWorldData", EditInstanceOnly, Export, Instanced, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = "true"))
	UComposite* WorldComposite;

	/** The planar reflection active for the composite. */
	UPROPERTY(Category = "PlanarReflectionWorldData", EditAnywhere)
	ACompositePlanarReflection* CompositePlanarReflection;
	
public:	
	/** Is the world composite applied to the view. */
	FORCEINLINE bool GetIsWorldCompositeEnabled() const { return bIsCompositingEnabled; };

	/** Visualize the composite meshes by projecting a world aligned grid. */
	FORCEINLINE bool GetDebugVisualizeCompositeMeshes() const { return bDebugVisualizeCompositeMeshes; }

	/** Output the shadows to the RGB for debugging. */
	FORCEINLINE bool GetDebugVisualizeShadows() const { return bDebugVisualizeShadows; }

	/** Output alpha in rgb for debugging. */
	FORCEINLINE bool GetDebugVisualizeAlphaInRgb() const { return bDebugVisualizeAlphaInRgb; }

	FORCEINLINE float GetDebugMediaOverlay() const { return DebugMediaOverlay; }
	
	/** Automatically pilot of the Compositor Preview camera during editor time. */
	FORCEINLINE bool GetAutoPilotEditorPreviewCamera() const { return bAutoPilotEditorPreviewCamera; }

	/** Automatically resize the viewport to the media texture input at runtime. */
	FORCEINLINE bool GetMatchViewportResolutionWithMediaInput() const { return bMatchViewportResolutionWithMediaInput; }

	/** Is camera motion blur enabled in the scene. */
	FORCEINLINE bool GetEnableCameraMotionBlur() const { return bEnableCameraMotionBlur; }

	/** The composite stored for this world. */
	FORCEINLINE UComposite* GetWorldComposite() const { return WorldComposite; }

	/** Get the camera responsible for projecting the media input during editor time. */
	FORCEINLINE ACameraActor* GetDebugEditorCamera() const { return DebugEditorCamera; }
	
	UFUNCTION(Category = "Compositor|Composite|WorldData")
	ACompositePlanarReflection* GetCompositePlanarReflection() const;

	UFUNCTION(Category = "Compositor|Composite|WorldData", BlueprintCallable)
	void SetCompositePlanarReflection(ACompositePlanarReflection* NewCompositePlanarReflection);

	bool IsAllowedToUseDebugEditorCamera() const;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif
};
