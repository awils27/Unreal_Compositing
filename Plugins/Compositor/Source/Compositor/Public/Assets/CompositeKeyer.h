// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeKeyer.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class UCompositorSubsystem;

/**
 * 
 */
UCLASS(Abstract, ClassGroup = Compositor, Category = "Compositor", BlueprintType, Blueprintable, EditInlineNew)
class COMPOSITOR_API UCompositeKeyer : public UObject
{
	GENERATED_BODY()
	
public:
    UCompositeKeyer(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	virtual void InitializeCompositeKeyer(UCompositorSubsystem* CompositorSubsystem);

	UFUNCTION()
	virtual void UpdateCompositeKeyer(UCompositorSubsystem* CompositorSubsystem);

	/** Return the CompositeKeyer material executed by this CompositeKeyer. */
	UFUNCTION(Category = "CompositeKeyer", BlueprintNativeEvent, BlueprintPure)
	UMaterialInterface* GetCompositeKeyerMaterial() const;

	/** Return the CompositeKeyer material instance dynamic executed by this CompositeKeyer. */
	UFUNCTION(Category = "CompositeKeyer", BlueprintPure)
	virtual UMaterialInstanceDynamic* GetCompositeKeyerMID();

	/** Event called when the CompositeKeyer is internally initialized. */
	UFUNCTION(Category = "CompositeKeyer", BlueprintImplementableEvent, meta=(DisplayName = "InitializeCompositeKeyer"))
	void ReceiveInitializeCompositeKeyer();

	/** Event called to update the CompositeKeyer. */
	UFUNCTION(Category = "CompositeKeyer", BlueprintImplementableEvent, meta=(DisplayName = "UpdateCompositeKeyer"))
	void ReceiveUpdateCompositeKeyer();

	/** Is the CompositeKeyer enabled. */
	UFUNCTION(Category = "CompositeKeyer")
	virtual bool GetIsKeyerEnabled();

private:
	/** Is the CompositeKeyer enabled. */
	UPROPERTY(Category = "CompositeKeyer", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool IsCompositeKeyerEnabled;

	/** The material instance dynamic created from the CompositeKeyer material. */
	UPROPERTY(Category = "CompositeKeyer", VisibleAnywhere, AdvancedDisplay, Transient)
	UMaterialInstanceDynamic* CompositeKeyerMID;

	UPROPERTY()
	UTextureRenderTarget2D* MediaInputKeyedRenderTarget;

	TMap<int32, FBoolProperty*> BoolParametersForMID;
	TMap<int32, float*> FloatParametersForMID;
	TMap<int32, FLinearColor*> LinearColorParametersForMID;
};
