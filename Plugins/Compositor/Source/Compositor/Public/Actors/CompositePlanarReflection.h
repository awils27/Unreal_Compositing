// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture.h"
#include "CompositePlanarReflection.generated.h"

class UCompositePlanarReflectionComponent;
class UBillboardComponent;
class UArrowComponent;

/**
 * 
 */
UCLASS()
class COMPOSITOR_API ACompositePlanarReflection : public ASceneCapture
{
	GENERATED_BODY()

private:

#if WITH_EDITORONLY_DATA
	// Reference to sprite visualization component
	UPROPERTY()
	class UBillboardComponent* SpriteComponent;

	// Reference to arrow visualization component
	UPROPERTY()
	class UArrowComponent* ArrowComponent;
#endif

public:
	/** Scene capture component. */
	UPROPERTY(Category = "CompositePlanarReflection", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCompositePlanarReflectionComponent* CompositePlanarReflectionComponent;

	/** Constructor */
	ACompositePlanarReflection(const FObjectInitializer& ObjectInitializer);

	/** Called when the object is created or when properties on the object change. */
	void OnConstruction(const FTransform& Transform);

	/** Makes this Planar Reflection the active one for the world composite. */
	UFUNCTION(Category = "Media Input", BlueprintCallable)
	virtual void SetActive();

	/** Returns CaptureComponent2D subobject **/
	UCompositePlanarReflectionComponent* GetCaptureComponent2D() const { return CompositePlanarReflectionComponent; }
};
