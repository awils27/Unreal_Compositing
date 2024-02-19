// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interfaces/CompositeUpdateInterface.h"

#include "CoreMinimal.h"
#include "CompositeTypes.h"
#include "GameFramework/Actor.h"

#include "CompositeMesh.generated.h"

UCLASS(Blueprintable, hidecategories=(Rendering,Replication,Input,Actor,Physics,Collision,Activation,Cooking,Navigation,VirtualTexture,AssetUserData,MaterialParameters,HLOD,LOD,Mobile,Materials))
class COMPOSITOR_API ACompositeMesh : public AActor, public ICompositeUpdateInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACompositeMesh(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;

	/** Overridable function called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(Category = "Composite Mesh", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* OpaqueMaterial;

	UPROPERTY(Category = "Composite Mesh", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* StencilMaterial;

	UPROPERTY(Category = "Composite Mesh", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* TranslucentMaterial;

	UPROPERTY(Category = "Composite Mesh", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* SoftMaskOpaqueMaterial;

	UPROPERTY(Category = "Composite Mesh", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* SoftMaskTranslucentMaterial;

private:

#if WITH_EDITORONLY_DATA
	// Reference to sprite visualization component
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> SpriteComponent;
#endif
	
	/** Created all the material instances and initializes the values. */
	UFUNCTION()
	void InitializeMaterialData();

	UFUNCTION()
	void InitializeUserProperties();

	UFUNCTION()
	void RegisterCompositeMesh();

	UFUNCTION()
	void UnregisterCompositeMesh();

	UFUNCTION()
	void AssignMaterialsToStaticMesh();

	UFUNCTION()
	void UpdateStencilValues();

	UPROPERTY(Category = "Composite Mesh", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneComponent;

	UPROPERTY(Category = "Composite Mesh", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* OpaqueComponent;

	UPROPERTY(Category = "Composite Mesh", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StencilComponent;

	UPROPERTY(Category = "Composite Mesh", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TranslucentComponent;

	UPROPERTY(Category = "Composite Mesh", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SoftMaskComponent;

	UPROPERTY(Category = "Composite Mesh", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* OpaqueMID;

	UPROPERTY(Category = "Composite Mesh", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* StencilMID;

	UPROPERTY(Category = "Composite Mesh", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* TranslucentMID;

	UPROPERTY(Category = "Composite Mesh", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* SoftMaskOpaqueMID;

	UPROPERTY(Category = "Composite Mesh", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* SoftMaskTranslucentMID;

	/** The static mesh on which the composite is applied. */
	UPROPERTY(Category = "Mesh", EditAnywhere, BlueprintSetter = SetStaticMesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* StaticMesh;

	/**
	 * Bypass the depth of field by projecting the vertices at the focus distance of the camera.
	 * Should only be necessary when you are keying and the silhouette of your talent is affected by the depth of field.
	 * The mesh can't be projected closer than the near clipping plane.
	 * If your focus distance plane clips with foreground elements make sure to manually assign foreground stencils to those actors like so:
	 * Right Click on Actor -> Scripted Actions -> Choose either Opaque or Translucent forgeround.
	 * Not recommended to use in a live environment due to the nasty side effects it can have with foreground elements.
	 * If your focus distance is in front of a foreground mesh the depth of field will look incorrect, hard edges around the foreground mesh even when it is out of focus.
	 * So when this is enabled always make sure your focus distance is in between your foreground mesh and your composite mesh.
	 * If your focus is further than you composite mesh it might still show artifacts. To completely get rid of those artifacts you can hide the Opaque Component which will cause issues with refraction and you won't have support for reflections and shadows on the mesh.
	 */
	UPROPERTY(Category = "Mesh", EditAnywhere, BlueprintSetter = SetBypassDepthOfField, AdvancedDisplay, meta = (AllowPrivateAccess = "true", DisplayName = "Bypass Depth of Field (unsafe)"))
	bool bBypassDepthOfField;
	/** Index of the BypassDepthOfField parameter. */
	int32 TranslucentBypassDepthOfFieldParameterIndex;

	/**
	 * Channels that this component should be in.  Lights with matching channels will affect the component.
	 * These channels only apply to opaque materials, direct lighting, and dynamic lighting and shadowing.
	 */
	UPROPERTY(Category = "Lighting", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FLightingChannels LightingChannels;

	/** Does this actor casts shadows. */
	UPROPERTY(Category = "Shadows", EditAnywhere, Interp, BlueprintSetter = SetCastShadows, meta = (AllowPrivateAccess = "true"))
	bool bCastShadows;

	/** Does this actor affect distance field lighting. */
	UPROPERTY(Category = "Shadows", EditAnywhere, Interp, BlueprintSetter = SetAffectDistanceFieldLighting, meta = (EditCondition = "bCastShadows", AllowPrivateAccess = "true"))
	bool bAffectDistanceFieldLighting;

	/** How visible the shadows are on the media. */
	UPROPERTY(Category = "Shadows", EditAnywhere, Interp, BlueprintSetter = SetReceiveShadowsIntensity, meta = (UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1", AllowPrivateAccess = "true"))
	float ReceiveShadowsIntensity;
	/** Index of the Shadow Intensity parameter. */
	int32 OpaqueShadowsIntensityParameterIndex;

	/**
		The intensity of the color itself over the media. 1 = Full color, 0 = black.
		Setting PlanarReflectionColorIntensity to 1 and PlanarReflectionMediaSuppression to 0 equals an additive reflection.
		Setting both PlanarReflectionColorIntensity and PlanarReflectionMediaSuppression to 1 equals an alpha blended reflection.
	*/
	UPROPERTY(Category = "Planar Reflection", EditAnywhere, Interp, BlueprintSetter = SetPlanarReflectionColorIntensity, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float PlanarReflectionColorIntensity;
	/** Index of the PlanarReflectionColorIntensity parameter. */
	int32 OpaquePlanarReflectionColorIntensityParameterIndex;

	/**
		Remove the background where the is reflection. This is useful when doing augmented reality on highly reflective mirror-like floors.
		Setting PlanarReflectionColorIntensity to 1 and PlanarReflectionMediaOcclusion to 0 equals an additive reflection.
		Setting both PlanarReflectionColorIntensity and PlanarReflectionMediaOcclusion to 1 equals an alpha blended reflection.
	*/
	UPROPERTY(Category = "Planar Reflection", EditAnywhere, Interp, BlueprintSetter = SetPlanarReflectionBackgroundOcclusion, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float PlanarReflectionBackgroundOcclusion;
	/** Index of the PlanarReflectionMediaOcclusion parameter. */
	int32 OpaquePlanarReflectionBackgroundOcclusionParameterIndex;

	/** Is this Composite Mesh rendering on both sides of the surface. */
	UPROPERTY(Category = "Shading", EditAnywhere, Interp, BlueprintSetter = SetIsTwoSided, meta = (AllowPrivateAccess = "true"))
	bool bIsTwoSided;
	/** Two sided parameter. */
	int32 OpaqueTwoSidedParameterIndex, StencilTwoSidedParameterIndex, TranslucentTwoSidedParameterIndex, SoftMaskOpaqueTwoSidedParameterIndex, SoftMaskTranslucentTwoSidedParameterIndex;

	/** Align the mesh normals with the first atmospheric light found in the scene, when not found this will be top down. When disabled it will use the mesh normal. */
	UPROPERTY(Category = "Shading", EditAnywhere, Interp, BlueprintSetter = SetAlignNormalsWithAtmosphereLight, meta = (AllowPrivateAccess = "true"))
	bool bAlignNormalsWithAtmosphereLight;
	/** AlignNormalsWithAtmosphereLight parameter. */
	int32 OpaqueAlignNormalsWithAtmosphereLightParameterIndex;

	/** The color of the back side of the mesh when it is ray traced.
	Alpha = 0: Color is multiplied with Media.
	Alpha = 1: Only the Color is visible. */
	UPROPERTY(Category = "Shading", EditAnywhere, Interp, BlueprintSetter = SetRayTracedBackfaceColor, AdvancedDisplay, meta = (AllowPrivateAccess = "true"))
	FLinearColor RayTracedBackfaceColor;
	/** Index of the RayTracedBackfaceColor parameter. */
	int32 OpaqueRayTracedBackfaceColorParameterIndex, TranslucentRayTracedBackfaceColorParameterIndex;

	/** The color of the mesh when it is ray traced outside of the camera frustum.
	Alpha = 0: Color is multiplied with Media.
	Alpha = 1: Only the Color is visible.
	This value only works for non keyed areas. */
	UPROPERTY(Category = "Shading", EditAnywhere, Interp, BlueprintSetter = SetRayTracedOutOfFrustumColor, AdvancedDisplay, meta = (AllowPrivateAccess = "true"))
	FLinearColor RayTracedOutOfFrustumColor;
	/** Index of the RayTracedBackfaceColor parameter. */
	int32 OpaqueRayTracedOutOfFrustumColorParameterIndex, TranslucentRayTracedOutOfFrustumColorParameterIndex;

	/** Is this Composite Mesh rendering to the soft mask. This allows for soft fading the mesh using vertex color alpha. */
	UPROPERTY(Category = "Soft Mask", EditAnywhere, Interp, BlueprintSetter = SetRenderSoftMask, meta = (AllowPrivateAccess = "true"))
	ERenderSoftMaskType RenderSoftMask;
	
	/** Soft mask parameter. */
	int32 OpaqueRenderSoftMaskParameterIndex, TranslucentRenderSoftMaskParameterIndex;

	int32 TranslucentRenderSoftMaskBlackParameterIndex;
	
	/**  */
	int32 SoftMaskOpaqueCustomValueParameterIndex, SoftMaskOpaqueRenderVertexColorParameterIndex;

	// Pointer to the world composite for quick access.
	UPROPERTY(Category = "Composite", BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = "true"))
	UComposite* WorldComposite;

public:
	/** Names used for the components. */
	static FName RootSceneComponentName;
	static FName OpaqueComponentName;
	static FName StencilComponentName;
	static FName TranslucentComponentName;
	static FName SoftMaskComponentName;

	// Stencil Value
	static int32 StencilValueOpaqueSoftMask;
	static int32 StencilValueTranslucentSoftMask;
	static int32 StencilValueOpaqueHardMask;
	static int32 StencilValueTranslucentHardMask;
	static int32 StencilValueOpaqueSoftMaskNoDoF;
	static int32 StencilValueTranslucentSoftMaskNoDoF;
	static int32 StencilValueOpaqueHardMaskNoDoF;
	static int32 StencilValueTranslucentHardMaskNoDoF;

	virtual void OnCompositeUpdate(UComposite& InWorldComposite) override;
	
	// All getter and setter functions.
	UStaticMeshComponent* GetSoftMaskComponent() const { return SoftMaskComponent; }

	/** The media mesh opaque material instance dynamic. */
	FORCEINLINE UMaterialInstanceDynamic* GetOpaqueMID() const { return OpaqueMID; };

	/** The media mesh stencil material instance dynamic. */
	FORCEINLINE UMaterialInstanceDynamic* GetStencilMID() const { return StencilMID; };

	/** The media translucent material instance dynamic. */
	FORCEINLINE UMaterialInstanceDynamic* GetTranslucentMID() const { return TranslucentMID; };

	/** The media soft mask material instance dynamic. */
	FORCEINLINE UMaterialInstanceDynamic* GetSoftMaskMID() const { return RenderSoftMask == ERenderSoftMaskType::TranslucentVertexColorAlpha ? SoftMaskTranslucentMID : SoftMaskOpaqueMID; }

	//
	FORCEINLINE UStaticMesh* GetStaticMesh() const { return StaticMesh; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetStaticMesh(UStaticMesh* NewStaticMesh);

	//
	FORCEINLINE bool GetBypassDepthOfField() const { return bBypassDepthOfField; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetBypassDepthOfField(const bool bNewBypassDepthOfField);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Components")
	void SetLightingChannels(const bool bChannel0, const bool bChannel1, const bool bChannel2);
	FORCEINLINE FLightingChannels GetLightingChannels() const { return LightingChannels; };

	FORCEINLINE float GetCastShadows() const { return bCastShadows; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetCastShadows(const bool bNewCastShadows);

	FORCEINLINE bool GetAffectDistanceFieldLighting() const { return bAffectDistanceFieldLighting; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetAffectDistanceFieldLighting(const bool bNewAffectDistanceFieldLighting);

	FORCEINLINE float GetReceiveShadowsIntensity() const { return ReceiveShadowsIntensity; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetReceiveShadowsIntensity(const float NewReceiveShadowsIntensity);

	FORCEINLINE float GetPlanarReflectionColorIntensity() const { return PlanarReflectionColorIntensity; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetPlanarReflectionColorIntensity(const float NewPlanarReflectionColorIntensity);

	FORCEINLINE float GetPlanarReflectionBackgroundOcclusion() const { return PlanarReflectionBackgroundOcclusion; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetPlanarReflectionBackgroundOcclusion(const float NewPlanarReflectionBackgroundOcclusion);

	FORCEINLINE bool GetIsTwoSided() const { return bIsTwoSided; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetIsTwoSided(const bool bNewIsTwoSided);

	FORCEINLINE bool GetAlignNormalsWithAtmosphereLight() const { return bAlignNormalsWithAtmosphereLight; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetAlignNormalsWithAtmosphereLight(const bool bNewAlignNormalsWithAtmosphereLight);

	FORCEINLINE FLinearColor GetRayTracedBackfaceColor() const { return RayTracedBackfaceColor; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetRayTracedBackfaceColor(const FLinearColor NewRayTracedBackfaceColor);

	FORCEINLINE FLinearColor GetRayTracedOutOfFrustumColor() const { return RayTracedOutOfFrustumColor; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetRayTracedOutOfFrustumColor(const FLinearColor NewRayTracedOutOfFrustumColor);

	FORCEINLINE ERenderSoftMaskType GetRenderSoftMask() const { return RenderSoftMask; };
	UFUNCTION(BlueprintSetter, meta = (CallInEditor = "true"))
	void SetRenderSoftMask(const ERenderSoftMaskType NewRenderSoftMaskType);

	bool AreAllComponentsValid() const;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
};
