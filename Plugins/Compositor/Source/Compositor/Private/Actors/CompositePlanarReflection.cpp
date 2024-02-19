// Copyright Epic Games, Inc. All Rights Reserved.


#include "Actors/CompositePlanarReflection.h"
#include "Subsystems/CompositorSubsystem.h"
#include "Objects/CompositeWorldData.h"
#include "Assets/Composite.h"
#include "Components/CompositePlanarReflectionComponent.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

ACompositePlanarReflection::ACompositePlanarReflection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CompositePlanarReflectionComponent = CreateDefaultSubobject<UCompositePlanarReflectionComponent>(TEXT("CompositePlanarReflectionComponent"));
	CompositePlanarReflectionComponent->SetupAttachment(RootComponent);


#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent0"));

	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTextureObject;
			FName ID_Compositor;
			FText NAME_Compositor;
			FConstructorStatics()
				: SpriteTextureObject(TEXT("/Engine/Tutorial/Basics/TutorialAssets/CameraActor_64x.CameraActor_64x"))
				, ID_Compositor(TEXT("Compositor"))
				, NAME_Compositor(NSLOCTEXT("CompositorCategory", "Compositor", "Compositor"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		if (SpriteComponent)
		{
			SpriteComponent->Sprite = ConstructorStatics.SpriteTextureObject.Get();
			SpriteComponent->SetRelativeScale3D_Direct(FVector(2.F));
			SpriteComponent->bHiddenInGame = true;
			SpriteComponent->bIsScreenSizeScaled = true;
			SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Compositor;
			SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Compositor;
			SpriteComponent->SetupAttachment(RootComponent);
			SpriteComponent->bReceivesDecals = false;
		}

		if (ArrowComponent)
		{
			ArrowComponent->ArrowColor = FColor(50, 100, 200);

			ArrowComponent->ArrowSize = 1.5f;
			ArrowComponent->bTreatAsASprite = true;
			ArrowComponent->bIsScreenSizeScaled = true;
			ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Compositor;
			ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Compositor;
			ArrowComponent->SetupAttachment(RootComponent);
			ArrowComponent->SetUsingAbsoluteScale(true);
			ArrowComponent->SetRelativeRotation(FRotator(90.F, 90.F, 90.F));
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void ACompositePlanarReflection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Makes sure to not register the transient asset used when dropping an actor into the world.
	if (HasAnyFlags(RF_Transient))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
		if (CompositorSubsystem)
		{
			UCompositeWorldData* CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
			if (CompositorSubsystem->GetCompositeWorldData())
			{
				// If no planar reflections are active in this world it will automatically assign this one.
				if (!CompositeWorldData->GetCompositePlanarReflection())
				{
					CompositeWorldData->SetCompositePlanarReflection(this);

					UComposite* CurrentComposite = CompositeWorldData->GetWorldComposite();
					if (CurrentComposite && CurrentComposite->GetEnablePlanarReflection() == false)
					{
						CurrentComposite->bOverride_EnablePlanarReflection = true;
						CurrentComposite->SetEnablePlanarReflection(true);
					}
				}
			}
		}
	}
}

void ACompositePlanarReflection::SetActive()
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
		if (CompositorSubsystem != nullptr)
		{
			UCompositeWorldData* CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
			if (CompositorSubsystem->GetCompositeWorldData() != nullptr)
			{
				CompositeWorldData->SetCompositePlanarReflection(this);				
			}
		}
	}	
}
