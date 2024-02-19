// Copyright Epic Games, Inc. All Rights Reserved.


#include "Components/CompositePlanarReflectionComponent.h"

#include "Actors/CompositePlanarReflection.h"
#include "Assets/Composite.h"
#include "Objects/CompositeWorldData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"

UCompositePlanarReflectionComponent::UCompositePlanarReflectionComponent()
{
	TextureTarget = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/CompositePlanarReflection/RT_CompositePlanarReflection.RT_CompositePlanarReflection")).TryLoad());

	ProfilingEventName = "Composite Planar Reflection";

	CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	ShowFlags.DeferredLighting = true;
	ShowFlags.SkyLighting = true;
	ShowFlags.ReflectionEnvironment = true;
	ShowFlags.Lighting = true;
	bEnableClipPlane = true;
}

float UCompositePlanarReflectionComponent::GetTargetTextureScreenPercentage() const
{
	if (WorldComposite)
	{
		return WorldComposite->GetPlanarReflectionScreenPercentage();
	}

	return Super::GetTargetTextureScreenPercentage();
}

bool UCompositePlanarReflectionComponent::GetIsCompositeCaptureActive() const
{
	if (WorldComposite && CompositeWorldData)
	{
		ACompositePlanarReflection* CompositePlanarReflection = CompositeWorldData->GetCompositePlanarReflection();
		if (CompositePlanarReflection)
		{
			const bool bIsActivePlanarReflection = CompositeWorldData->GetCompositePlanarReflection()->CompositePlanarReflectionComponent == this;
			return WorldComposite->GetEnablePlanarReflection() && bIsActivePlanarReflection;
		}
	}
	
	return false;
}

void UCompositePlanarReflectionComponent::UpdateCameraData(FVector& OutLocation, FRotator& OutRotation, float& OutFieldOfView, FVector& OutClipPlaneBase, FVector& OutClipBaseNormal) const
{
	Super::UpdateCameraData(OutLocation, OutRotation, OutFieldOfView, OutClipPlaneBase, OutClipBaseNormal);

	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const FRotator ActorRotation = GetOwner()->GetActorRotation();

	OutClipPlaneBase = ActorLocation;
	const FTransform BaseTransform = FTransform(ActorRotation, ActorLocation, FVector::OneVector);
	const FTransform FlippedTransform = FTransform(BaseTransform.Rotator(), BaseTransform.GetLocation(), FVector(1.F, 1.F, -1.F));

	OutLocation = BaseTransform.InverseTransformPosition(OutLocation);
	OutLocation = FlippedTransform.TransformPosition(OutLocation);

	const FVector PlaneNormal = BaseTransform.GetRotation().GetUpVector();
	OutClipBaseNormal = PlaneNormal;

	const FVector X = UKismetMathLibrary::MirrorVectorByNormal(UKismetMathLibrary::GetForwardVector(OutRotation), PlaneNormal);
	const FVector Y = UKismetMathLibrary::MirrorVectorByNormal(UKismetMathLibrary::GetRightVector(OutRotation), PlaneNormal);
	OutRotation = UKismetMathLibrary::MakeRotFromXY(X, Y);
}
