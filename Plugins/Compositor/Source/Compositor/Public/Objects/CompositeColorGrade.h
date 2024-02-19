// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CompositeColorGrade.generated.h"

/**
 * Wrapper class for all color grading properties.
 */
UCLASS(ClassGroup = Compositor, Category = "Compositor", BlueprintType, Blueprintable, EditInlineNew)
class COMPOSITOR_API UCompositeColorGrade : public UObject
{
	GENERATED_BODY()

public:
	UCompositeColorGrade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color Grading", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ColorGradeScene : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color Grading", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ColorGradeMedia : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color Grading", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ColorGradeCombined : 1;

private:
	///**
	//Color correction controls for adjustments on the scene color.
	//This is applied at the start of the post processing pipeline.
	//This means the media matte will not be affected by it.
	//Because the media matte is applied to the scene after this color correction.
	//The standard unreal color correction found in the post process volume might cause artifacts on the matte.
	//*/
	UPROPERTY(Category = "Color Grading", EditAnywhere, meta = (EditCondition = "bOverride_ColorGradeScene"))
	FColorGradePerRangeSettings ColorGradeScene;

	/** Color correction controls for adjustments on the media. This is applied after keying. */
	UPROPERTY(Category = "Color Grading", EditAnywhere, meta = (EditCondition = "bOverride_ColorGradeMedia"))
	FColorGradePerRangeSettings ColorGradeMedia;

	/** The color grade happens after the media input has been combined with the virtual scene. */
	UPROPERTY(Category = "Color Grading", EditAnywhere, meta = (EditCondition = "bOverride_ColorGradeCombined"))
	FColorGradePerRangeSettings ColorGradeCombined;

	UPROPERTY()
	UCompositeColorGrade* CompositeColorGradeClassDefaults;

public:
	UFUNCTION(Category = "Color Grading", BlueprintPure)
	FColorGradePerRangeSettings GetColorGradeScene() const;

	UFUNCTION(Category = "Color Grading", BlueprintCallable)
	void SetColorGradeScene(FColorGradePerRangeSettings NewColorGradeScene);

	UFUNCTION(Category = "Color Grading", BlueprintPure)
	FColorGradePerRangeSettings GetColorGradeMedia() const;

	UFUNCTION(Category = "Color Grading", BlueprintCallable)
	void SetColorGradeMedia(FColorGradePerRangeSettings NewColorGradeMedia);

	UFUNCTION(Category = "Color Grading", BlueprintPure)
	FColorGradePerRangeSettings GetColorGradeCombined() const;

	UFUNCTION(Category = "Color Grading", BlueprintCallable)
	void SetColorGradeCombined(FColorGradePerRangeSettings NewColorGradeCombined);
};
