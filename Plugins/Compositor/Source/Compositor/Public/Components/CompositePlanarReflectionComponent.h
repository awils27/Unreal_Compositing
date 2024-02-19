// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeCaptureComponent2D.h"
#include "CompositePlanarReflectionComponent.generated.h"

/**
 * 
 */
UCLASS()
class COMPOSITOR_API UCompositePlanarReflectionComponent : public UCompositeCaptureComponent2D
{
	GENERATED_BODY()

public:
	UCompositePlanarReflectionComponent();

	virtual float GetTargetTextureScreenPercentage() const override;

	virtual bool GetIsCompositeCaptureActive() const;

protected:
	virtual void UpdateCameraData(FVector& OutLocation, FRotator& OutRotation, float& OutFieldOfView, FVector& OutClipPlaneBase, FVector& OutClipBaseNormal) const override;
};
