// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Interfaces/CompositeUpdateInterface.h"

#if WITH_EDITOR
#include "LevelEditorViewport.h"
#endif

#include "CompositeCaptureComponent2D.generated.h"

class APlayerCameraManager;
class UCameraComponent;

class UCompositorSubsystem;
class UComposite;
class UCompositeWorldData;

/**
 * 
 */
UCLASS()
class COMPOSITOR_API UCompositeCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UCompositeCaptureComponent2D();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void BeginPlay() override;

	virtual float GetTargetTextureScreenPercentage() const;

	virtual bool GetIsCompositeCaptureActive() const;

	virtual void UpdateRenderTargetSize();

	virtual void OnCompositeUpdateInterfaceRegistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface){}
	virtual void OnCompositeUpdateInterfaceUnregistered(TScriptInterface<ICompositeUpdateInterface> CompositeUpdateInterface){}

protected:
	UPROPERTY(Transient)
	bool bAllowDebugEditorCamera;
	
	UPROPERTY(Transient)
	APlayerCameraManager* PlayerCameraManager;

#if WITH_EDITORONLY_DATA
	FLevelEditorViewportClient* LevelEditorViewportClient;

	UPROPERTY(Transient)
	UCameraComponent* EditorCameraComponent;
#endif

	UPROPERTY(Transient)
	UCompositorSubsystem* CompositorSubsystem;

	UPROPERTY(Transient)
	UComposite* WorldComposite;

	UPROPERTY(Transient)
	UCompositeWorldData* CompositeWorldData;

	FVector PlanarCameraLocation;
	FRotator PlanarCameraRotation;
	
	virtual void UpdateCameraData(FVector& OutLocation, FRotator& OutRotation, float& OutFieldOfView, FVector& OutClipPlaneBase, FVector& OutClipBaseNormal) const;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateSceneCaptureContents(FSceneInterface* Scene) override;

private:

	float PreviousScreenPercentage;
};
