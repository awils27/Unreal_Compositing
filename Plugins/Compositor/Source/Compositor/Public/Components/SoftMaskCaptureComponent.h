// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeCaptureComponent2D.h"
#include "SoftMaskCaptureComponent.generated.h"

/**
 * 
 */
UCLASS()
class COMPOSITOR_API USoftMaskCaptureComponent : public UCompositeCaptureComponent2D
{
	GENERATED_BODY()

public:
	USoftMaskCaptureComponent();

	virtual bool GetIsCompositeCaptureActive() const override;

	virtual void OnCompositeUpdateInterfaceRegistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface) override;
	virtual void OnCompositeUpdateInterfaceUnregistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface) override;

	virtual float GetTargetTextureScreenPercentage() const override;
};
