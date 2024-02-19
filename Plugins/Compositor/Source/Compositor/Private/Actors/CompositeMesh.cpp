// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/CompositeMesh.h"
#include "Assets/Composite.h"
#include "Components/BillboardComponent.h"
#include "Subsystems/CompositorSubsystem.h"
#include "Components/SoftMaskCaptureComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Objects/CompositeWorldData.h"

FName ACompositeMesh::RootSceneComponentName(TEXT("RootSceneComponent"));
FName ACompositeMesh::OpaqueComponentName(TEXT("OpaqueComponent"));
FName ACompositeMesh::StencilComponentName(TEXT("StencilComponent"));
FName ACompositeMesh::TranslucentComponentName(TEXT("TranslucentComponent"));
FName ACompositeMesh::SoftMaskComponentName(TEXT("SoftMaskComponent"));

int32 ACompositeMesh::StencilValueTranslucentSoftMask(255);
int32 ACompositeMesh::StencilValueOpaqueSoftMask(254);
int32 ACompositeMesh::StencilValueTranslucentHardMask(253);
int32 ACompositeMesh::StencilValueOpaqueHardMask(252);

int32 ACompositeMesh::StencilValueTranslucentSoftMaskNoDoF(251);
int32 ACompositeMesh::StencilValueOpaqueSoftMaskNoDoF(250);
int32 ACompositeMesh::StencilValueTranslucentHardMaskNoDoF(249);
int32 ACompositeMesh::StencilValueOpaqueHardMaskNoDoF(248);

// Sets default values
ACompositeMesh::ACompositeMesh(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = Cast<UStaticMesh>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/SM_DefaultCompositeMesh.SM_DefaultCompositeMesh")).TryLoad());

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(RootSceneComponentName);
	RootSceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = RootSceneComponent;

	OpaqueMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/MI_CompositeMeshOpaque.MI_CompositeMeshOpaque")).TryLoad());
	StencilMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/MI_CompositeMeshStencil.MI_CompositeMeshStencil")).TryLoad());
	TranslucentMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/MI_CompositeMeshTranslucent.MI_CompositeMeshTranslucent")).TryLoad());
	SoftMaskOpaqueMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/MI_CompositeMeshSoftMaskOpaque.MI_CompositeMeshSoftMaskOpaque")).TryLoad());
	SoftMaskTranslucentMaterial = Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Compositor/CompositeMesh/MI_CompositeMeshSoftMaskTranslucent.MI_CompositeMeshSoftMaskTranslucent")).TryLoad());
	
	OpaqueComponent = CreateDefaultSubobject<UStaticMeshComponent>(OpaqueComponentName);
	OpaqueComponent->Mobility = EComponentMobility::Static;
	OpaqueComponent->SetupAttachment(RootComponent);
	OpaqueComponent->bRenderInMainPass = true;
	OpaqueComponent->bRenderInDepthPass = true;
	OpaqueComponent->bRenderCustomDepth = true;
	OpaqueComponent->CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_Default;
	OpaqueComponent->CustomDepthStencilValue = StencilValueOpaqueSoftMask;
	OpaqueComponent->bVisibleInReflectionCaptures = false;
	OpaqueComponent->bVisibleInRealTimeSkyCaptures = false;
	OpaqueComponent->bCastStaticShadow = false;
	OpaqueComponent->bCastContactShadow = false;

	StencilComponent = CreateDefaultSubobject<UStaticMeshComponent>(StencilComponentName);
	StencilComponent->Mobility = EComponentMobility::Static;
	StencilComponent->SetupAttachment(OpaqueComponent);
	StencilComponent->SetCastShadow(false);
	StencilComponent->bRenderInMainPass = false;
	StencilComponent->bRenderInDepthPass = false;
	StencilComponent->bRenderCustomDepth = true;
	StencilComponent->CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_Default;
	StencilComponent->CustomDepthStencilValue = StencilValueTranslucentSoftMask;
	StencilComponent->bVisibleInReflectionCaptures = false;
	StencilComponent->bVisibleInRealTimeSkyCaptures = false;
	StencilComponent->bVisibleInRayTracing = false;

	TranslucentComponent = CreateDefaultSubobject<UStaticMeshComponent>(TranslucentComponentName);
	TranslucentComponent->Mobility = EComponentMobility::Static;
	TranslucentComponent->SetupAttachment(OpaqueComponent);
	TranslucentComponent->SetCastShadow(false);
	TranslucentComponent->bVisibleInReflectionCaptures = false;
	TranslucentComponent->bVisibleInRealTimeSkyCaptures = false;

	SoftMaskComponent = CreateDefaultSubobject<UStaticMeshComponent>(SoftMaskComponentName);
	SoftMaskComponent->Mobility = EComponentMobility::Static;
	SoftMaskComponent->SetupAttachment(OpaqueComponent);
	SoftMaskComponent->SetCastShadow(false);
	SoftMaskComponent->bVisibleInReflectionCaptures = false;
	SoftMaskComponent->bVisibleInRealTimeSkyCaptures = false;
	SoftMaskComponent->bVisibleInRayTracing = false;
	SoftMaskComponent->bVisibleInSceneCaptureOnly = true;

	PlanarReflectionColorIntensity = 1.F;
	bAlignNormalsWithAtmosphereLight = true;

	RenderSoftMask = ERenderSoftMaskType::OpaqueWhite;
	
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	
	if (!IsRunningCommandlet())
	{
		if (SpriteComponent)
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTextureObject = TEXT("/Engine/EditorResources/S_SceneCaptureIcon");
			SpriteComponent->Sprite = SpriteTextureObject.Get();
			SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
			SpriteComponent->bHiddenInGame = true;
			SpriteComponent->bIsScreenSizeScaled = true;
			SpriteComponent->SetupAttachment(RootSceneComponent);
			SpriteComponent->bReceivesDecals = false;
		}
	}
#endif
}

void ACompositeMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	StencilComponent->SetRelativeTransform(FTransform::Identity);
	TranslucentComponent->SetRelativeTransform(FTransform::Identity);
	SoftMaskComponent->SetRelativeTransform(FTransform::Identity);
	StencilComponent->SetMobility(OpaqueComponent->Mobility);
	TranslucentComponent->SetMobility(OpaqueComponent->Mobility);
	SoftMaskComponent->SetMobility(OpaqueComponent->Mobility);

	if (WorldComposite == nullptr)
	{
		RegisterCompositeMesh();// Also calls OnCompositeUpdate_Implementation;
	}

	if (!OpaqueMID || OpaqueMID->Parent != OpaqueMaterial ||
		!StencilMID || StencilMID->Parent != StencilMaterial ||
		!TranslucentMID || TranslucentMID->Parent != TranslucentMaterial ||
		!SoftMaskTranslucentMID || SoftMaskTranslucentMID->Parent != SoftMaskTranslucentMaterial)
	{
		InitializeMaterialData();
	}

	InitializeUserProperties();
}

#if WITH_EDITOR
void ACompositeMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// These setters are called to make sure the behavior of changing a property is always the same.
	// Sequencer updates the setter automatically because it finds the function based on Set<PropName>(float InFoo) and the function is tagged as meta = (CallInEditor = "true").
	// Blueprint calls it because the function is marked as the BlueprintSetter for the property.
	// The details panel doesn't follow any of those setters so we manually call the setter just so it is in sync (i.e. if it also needs to change a material parameter based on the changed property)
	InitializeUserProperties();
}
#endif

void ACompositeMesh::BeginPlay()
{
	RegisterCompositeMesh();
	InitializeMaterialData();
	InitializeUserProperties();

	Super::BeginPlay();
}

void ACompositeMesh::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterCompositeMesh();

	Super::EndPlay(EndPlayReason);
}

// Is also called directly after this CompositeUpdateInterface is registered in the CompositorSubsystem.
void ACompositeMesh::OnCompositeUpdate(UComposite& InWorldComposite)
{
	if (WorldComposite != &InWorldComposite)
	{
		WorldComposite = &InWorldComposite;
	}

	// Update the receive shadows since it uses the global parameters as well.
	SetReceiveShadowsIntensity(ReceiveShadowsIntensity);
}

void ACompositeMesh::InitializeMaterialData()
{
	OpaqueMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, OpaqueMaterial, NAME_None, EMIDCreationFlags::Transient);	
	check(OpaqueMID)	
	OpaqueMID->InitializeScalarParameterAndGetIndex("ShadowsIntensity", ReceiveShadowsIntensity, OpaqueShadowsIntensityParameterIndex);
	OpaqueMID->InitializeScalarParameterAndGetIndex("PlanarReflectionColorIntensity", PlanarReflectionColorIntensity, OpaquePlanarReflectionColorIntensityParameterIndex);
	OpaqueMID->InitializeScalarParameterAndGetIndex("PlanarReflectionBackgroundOcclusion", PlanarReflectionBackgroundOcclusion, OpaquePlanarReflectionBackgroundOcclusionParameterIndex);
	OpaqueMID->InitializeScalarParameterAndGetIndex("IsTwoSided", bIsTwoSided ? 1.F : 0.F, OpaqueTwoSidedParameterIndex);
	OpaqueMID->InitializeScalarParameterAndGetIndex("AlignNormalsWithAtmosphereLight", bAlignNormalsWithAtmosphereLight ? 1.F : 0.F, OpaqueAlignNormalsWithAtmosphereLightParameterIndex);
	OpaqueMID->InitializeVectorParameterAndGetIndex("RayTracedBackfaceColor", RayTracedBackfaceColor, OpaqueRayTracedBackfaceColorParameterIndex);
	OpaqueMID->InitializeVectorParameterAndGetIndex("RayTracedOutOfFrustumColor", RayTracedOutOfFrustumColor, OpaqueRayTracedOutOfFrustumColorParameterIndex);
	OpaqueMID->InitializeScalarParameterAndGetIndex("RenderSoftMask", 1.F, OpaqueRenderSoftMaskParameterIndex);

	StencilMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, StencilMaterial, NAME_None, EMIDCreationFlags::Transient);
	check(StencilMID)
	StencilMID->InitializeScalarParameterAndGetIndex("IsTwoSided", bIsTwoSided ? 1.F : 0.F, StencilTwoSidedParameterIndex);

	TranslucentMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, TranslucentMaterial, NAME_None, EMIDCreationFlags::Transient);
	check(TranslucentMID)
	TranslucentMID->InitializeScalarParameterAndGetIndex("IsTwoSided", bIsTwoSided ? 1.F : 0.F, TranslucentTwoSidedParameterIndex);
	TranslucentMID->InitializeVectorParameterAndGetIndex("RayTracedBackfaceColor", RayTracedBackfaceColor, TranslucentRayTracedBackfaceColorParameterIndex);
	TranslucentMID->InitializeVectorParameterAndGetIndex("RayTracedOutOfFrustumColor", RayTracedOutOfFrustumColor, TranslucentRayTracedOutOfFrustumColorParameterIndex);
	TranslucentMID->InitializeScalarParameterAndGetIndex("BypassDepthOfField", bBypassDepthOfField ? 1.F : 0.F, TranslucentBypassDepthOfFieldParameterIndex);
	TranslucentMID->InitializeScalarParameterAndGetIndex("RenderSoftMask", 1.F, TranslucentRenderSoftMaskParameterIndex);
	TranslucentMID->InitializeScalarParameterAndGetIndex("RenderSoftMaskBlack", 0.F, TranslucentRenderSoftMaskBlackParameterIndex);

	SoftMaskOpaqueMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SoftMaskOpaqueMaterial, NAME_None, EMIDCreationFlags::Transient);
	check(SoftMaskOpaqueMID)
	SoftMaskOpaqueMID->InitializeScalarParameterAndGetIndex("IsTwoSided", bIsTwoSided ? 1.F : 0.F, SoftMaskOpaqueTwoSidedParameterIndex);
	SoftMaskOpaqueMID->InitializeScalarParameterAndGetIndex("CustomValue", 1.F, SoftMaskOpaqueCustomValueParameterIndex);
	SoftMaskOpaqueMID->InitializeScalarParameterAndGetIndex("RenderVertexColor", 1.F, SoftMaskOpaqueRenderVertexColorParameterIndex);

	SoftMaskTranslucentMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, SoftMaskTranslucentMaterial, NAME_None, EMIDCreationFlags::Transient);
	check(SoftMaskTranslucentMID)
	SoftMaskTranslucentMID->InitializeScalarParameterAndGetIndex("IsTwoSided", bIsTwoSided ? 1.F : 0.F, SoftMaskTranslucentTwoSidedParameterIndex);

	AssignMaterialsToStaticMesh();
}

void ACompositeMesh::InitializeUserProperties()
{
	SetStaticMesh(StaticMesh);
	SetBypassDepthOfField(bBypassDepthOfField);
	SetLightingChannels(LightingChannels.bChannel0, LightingChannels.bChannel1, LightingChannels.bChannel2);
	SetCastShadows(bCastShadows);
	SetAffectDistanceFieldLighting(bAffectDistanceFieldLighting);
	SetReceiveShadowsIntensity(ReceiveShadowsIntensity);
	SetPlanarReflectionColorIntensity(PlanarReflectionColorIntensity);
	SetPlanarReflectionBackgroundOcclusion(PlanarReflectionBackgroundOcclusion);
	SetIsTwoSided(bIsTwoSided);
	SetRenderSoftMask(RenderSoftMask);
	SetRayTracedBackfaceColor(RayTracedBackfaceColor);
	SetRayTracedOutOfFrustumColor(RayTracedOutOfFrustumColor);
	SetAlignNormalsWithAtmosphereLight(bAlignNormalsWithAtmosphereLight);
}

void ACompositeMesh::RegisterCompositeMesh()
{
	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
		if (IsValid(CompositorSubsystem))
		{
			CompositorSubsystem->RegisterCompositeUpdateInterface(this);
		}
	}
}

void ACompositeMesh::UnregisterCompositeMesh()
{
	// Unregister Composite Mesh from the compositor subsystem.
	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
		if (IsValid(CompositorSubsystem))
		{
			CompositorSubsystem->UnregisterCompositeUpdateInterface(this);
		}
	}
}

void ACompositeMesh::AssignMaterialsToStaticMesh()
{
	if (AreAllComponentsValid())
	{
		for (int32 i = 0; i < OpaqueComponent->GetNumMaterials(); i++)
		{
			OpaqueComponent->SetMaterial(i, GetOpaqueMID());
			StencilComponent->SetMaterial(i, GetStencilMID());
			TranslucentComponent->SetMaterial(i, GetTranslucentMID());
			SoftMaskComponent->SetMaterial(i, GetSoftMaskMID());
		}
	}
}

void ACompositeMesh::UpdateStencilValues()
{
	if (IsValid(OpaqueComponent))
	{
		if (bBypassDepthOfField)
		{
			OpaqueComponent->CustomDepthStencilValue = StencilValueOpaqueSoftMaskNoDoF;//(RenderSoftMask != ERenderSoftMaskType::Off ? StencilValueOpaqueSoftMaskNoDoF : StencilValueOpaqueHardMaskNoDoF);
		}
		else
		{
			OpaqueComponent->CustomDepthStencilValue = StencilValueOpaqueSoftMask;//StencilValueOpaqueSoftMask;(RenderSoftMask != ERenderSoftMaskType::Off ? StencilValueOpaqueSoftMask : StencilValueOpaqueHardMask);
		}
	}

	if (IsValid(StencilComponent))
	{
		if (bBypassDepthOfField)
		{
			StencilComponent->CustomDepthStencilValue = StencilValueTranslucentSoftMaskNoDoF;//(RenderSoftMask != ERenderSoftMaskType::Off ? StencilValueTranslucentSoftMaskNoDoF : StencilValueTranslucentHardMaskNoDoF);
		}
		else
		{
			StencilComponent->CustomDepthStencilValue = StencilValueTranslucentSoftMask;//(RenderSoftMask != ERenderSoftMaskType::Off ? StencilValueTranslucentSoftMask : StencilValueTranslucentHardMask);
		}
	}
}

void ACompositeMesh::SetStaticMesh(UStaticMesh* NewStaticMesh)
{
	StaticMesh = NewStaticMesh;

	if (IsValid(OpaqueComponent) && OpaqueComponent->GetStaticMesh() != StaticMesh)
	{
		OpaqueComponent->SetStaticMesh(StaticMesh);
	}

	if (IsValid(StencilComponent) && StencilComponent->GetStaticMesh() != StaticMesh)
	{
		StencilComponent->SetStaticMesh(StaticMesh);
	}

	if (IsValid(TranslucentComponent) && TranslucentComponent->GetStaticMesh() != StaticMesh)
	{
		TranslucentComponent->SetStaticMesh(StaticMesh);
	}

	if (IsValid(SoftMaskComponent) && SoftMaskComponent->GetStaticMesh() != StaticMesh)
	{
		SoftMaskComponent->SetStaticMesh(StaticMesh);
	}

	AssignMaterialsToStaticMesh();
}

void ACompositeMesh::SetBypassDepthOfField(const bool bNewBypassDepthOfField)
{
	bBypassDepthOfField = bNewBypassDepthOfField;
	TranslucentMID->SetScalarParameterByIndex(TranslucentBypassDepthOfFieldParameterIndex, bBypassDepthOfField);
	UpdateStencilValues();
}

void ACompositeMesh::SetLightingChannels(const bool bChannel0, const bool bChannel1, const bool bChannel2)
{
	OpaqueComponent->SetLightingChannels(bChannel0, bChannel1, bChannel2);
	TranslucentComponent->SetLightingChannels(bChannel0, bChannel1, bChannel2);
}

void ACompositeMesh::SetCastShadows(const bool bNewCastShadows)
{
	bCastShadows = bNewCastShadows;
	OpaqueComponent->SetCastShadow(bCastShadows);
}

void ACompositeMesh::SetAffectDistanceFieldLighting(const bool bNewAffectDistanceFieldLighting)
{
	bAffectDistanceFieldLighting = bNewAffectDistanceFieldLighting;
	OpaqueComponent->bAffectDistanceFieldLighting = bAffectDistanceFieldLighting;
}

void ACompositeMesh::SetReceiveShadowsIntensity(const float NewReceiveShadowsIntensity)
{
	if (IsValid(OpaqueMID) && IsValid(WorldComposite))
	{
		ReceiveShadowsIntensity = NewReceiveShadowsIntensity;

		const float ShadowIntensityMaterial = ReceiveShadowsIntensity * (WorldComposite->GetEnableMediaShadows() ? 1.F : 0.F);
		OpaqueMID->SetScalarParameterByIndex(OpaqueShadowsIntensityParameterIndex, ShadowIntensityMaterial);
	}
}

void ACompositeMesh::SetPlanarReflectionColorIntensity(const float NewPlanarReflectionColorIntensity)
{
	if (IsValid(OpaqueMID))
	{
		PlanarReflectionColorIntensity = NewPlanarReflectionColorIntensity;
		OpaqueMID->SetScalarParameterByIndex(OpaquePlanarReflectionColorIntensityParameterIndex, PlanarReflectionColorIntensity);
	}
}

void ACompositeMesh::SetPlanarReflectionBackgroundOcclusion(const float NewPlanarReflectionBackgroundOcclusion)
{
	if (IsValid(OpaqueMID))
	{
		PlanarReflectionBackgroundOcclusion = NewPlanarReflectionBackgroundOcclusion;
		OpaqueMID->SetScalarParameterByIndex(OpaquePlanarReflectionBackgroundOcclusionParameterIndex, PlanarReflectionBackgroundOcclusion);
	}
}

void ACompositeMesh::SetIsTwoSided(const bool bNewIsTwoSided)
{
	if (IsValid(OpaqueMID) && IsValid(StencilMID) && IsValid(TranslucentMID) && IsValid(SoftMaskOpaqueMID) && IsValid(SoftMaskTranslucentMID))
	{
		bIsTwoSided = bNewIsTwoSided;
		const float IsTwoSidedFloat = bIsTwoSided ? 1.F : 0.F;
		
		OpaqueMID->SetScalarParameterByIndex(OpaqueTwoSidedParameterIndex, IsTwoSidedFloat);
		StencilMID->SetScalarParameterByIndex(StencilTwoSidedParameterIndex, IsTwoSidedFloat);
		TranslucentMID->SetScalarParameterByIndex(TranslucentTwoSidedParameterIndex, IsTwoSidedFloat);
		SoftMaskOpaqueMID->SetScalarParameterByIndex(SoftMaskOpaqueTwoSidedParameterIndex, IsTwoSidedFloat);
		SoftMaskTranslucentMID->SetScalarParameterByIndex(SoftMaskTranslucentTwoSidedParameterIndex, IsTwoSidedFloat);
	}
}

void ACompositeMesh::SetAlignNormalsWithAtmosphereLight(const bool bNewAlignNormalsWithAtmosphereLight)
{
	if (IsValid(OpaqueMID))
	{
		bAlignNormalsWithAtmosphereLight = bNewAlignNormalsWithAtmosphereLight;

		OpaqueMID->SetScalarParameterByIndex(OpaqueAlignNormalsWithAtmosphereLightParameterIndex, bAlignNormalsWithAtmosphereLight ? 1.F : 0.F);
	}
}

void ACompositeMesh::SetRayTracedBackfaceColor(const FLinearColor NewRayTracedBackfaceColor)
{
	if (IsValid(OpaqueMID) && IsValid(TranslucentMID))
	{
		RayTracedBackfaceColor = NewRayTracedBackfaceColor;

		OpaqueMID->SetVectorParameterByIndex(OpaqueRayTracedBackfaceColorParameterIndex, RayTracedBackfaceColor);
		TranslucentMID->SetVectorParameterByIndex(TranslucentRayTracedBackfaceColorParameterIndex, RayTracedBackfaceColor);
	}
}

void ACompositeMesh::SetRayTracedOutOfFrustumColor(const FLinearColor NewRayTracedOutOfFrustumColor)
{
	if (IsValid(OpaqueMID) && IsValid(TranslucentMID))
	{
		RayTracedOutOfFrustumColor = NewRayTracedOutOfFrustumColor;

		OpaqueMID->SetVectorParameterByIndex(OpaqueRayTracedOutOfFrustumColorParameterIndex, RayTracedOutOfFrustumColor);
		TranslucentMID->SetVectorParameterByIndex(TranslucentRayTracedOutOfFrustumColorParameterIndex, RayTracedOutOfFrustumColor);
	}
}

void ACompositeMesh::SetRenderSoftMask(const ERenderSoftMaskType NewRenderSoftMaskType)
{
	RenderSoftMask = NewRenderSoftMaskType;
	const float RenderSoftMaskAsFloat = 1.F;
	const bool bRenderOpaqueBlackSoftMask = RenderSoftMask == ERenderSoftMaskType::OpaqueBlack;

	if (IsValid(OpaqueMID))
	{
		OpaqueMID->SetScalarParameterByIndex(OpaqueRenderSoftMaskParameterIndex, RenderSoftMaskAsFloat);
		SoftMaskOpaqueMID->SetScalarParameterByIndex(SoftMaskOpaqueCustomValueParameterIndex, RenderSoftMask == ERenderSoftMaskType::OpaqueWhite ? 1.F : 0.F);
		SoftMaskOpaqueMID->SetScalarParameterByIndex(SoftMaskOpaqueRenderVertexColorParameterIndex, RenderSoftMask == ERenderSoftMaskType::OpaqueVertexColorAlpha ? 1.F : 0.F);
	}

	if (IsValid(TranslucentMID))
	{
		TranslucentMID->SetScalarParameterByIndex(TranslucentRenderSoftMaskParameterIndex, RenderSoftMaskAsFloat);
		TranslucentMID->SetScalarParameterByIndex(TranslucentRenderSoftMaskBlackParameterIndex, bRenderOpaqueBlackSoftMask);
	}

	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		const UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>(World);
		if (IsValid(CompositorSubsystem))
		{
			USoftMaskCaptureComponent* SoftMaskCaptureComponent = CompositorSubsystem->GetSoftMaskCaptureComponent();
			if (IsValid(SoftMaskCaptureComponent))
			{
				//if (RenderSoftMask != ERenderSoftMaskType::Off)
				//{
					SoftMaskCaptureComponent->ShowOnlyComponent(SoftMaskComponent);
				//}
				//else
				//{
				//	SoftMaskCaptureComponent->RemoveShowOnlyComponent(SoftMaskComponent);
				//}
			}
		}
	}

	OpaqueComponent->SetVisibility(!bRenderOpaqueBlackSoftMask);
	TranslucentComponent->SetVisibility(true);
	StencilComponent->SetVisibility(!bRenderOpaqueBlackSoftMask);
	
	UpdateStencilValues();
}

bool ACompositeMesh::AreAllComponentsValid() const
{
	return IsValid(OpaqueComponent) && IsValid(StencilComponent) && IsValid(TranslucentComponent) && IsValid(SoftMaskComponent);
}

#if WITH_EDITOR
bool ACompositeMesh::CanEditChange(const FProperty* InProperty) const
{
	if (InProperty)
	{
		FString PropertyName = InProperty->GetName();

		// For now the opaque mesh is shown when the bypass depth of field is on, but lets leave this here in case we have to turn it off.
		//if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ACompositeMesh, ReceiveShadowsIntensity)
		//	|| PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ACompositeMesh, PlanarReflectionColorIntensity)
		//	|| PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ACompositeMesh, PlanarReflectionBackgroundOcclusion)
		//	)
		//{
		//	return !GetBypassDepthOfField();
		//}
	}

	return Super::CanEditChange(InProperty);
	
}
#endif
