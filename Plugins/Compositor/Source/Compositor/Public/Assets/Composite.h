// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeTypes.h"
#include "Engine/Scene.h" // FColorGradePerRange
#include "Composite.generated.h"

class UMediaTexture;
class UCompositeKeyer;
class UCompositeColorGrade;
class UUserWidget;

/**
 * 
 */
UCLASS(ClassGroup = Compositor, Category = "Compositor", BlueprintType, Blueprintable, EditInlineNew)
class COMPOSITOR_API UComposite : public UObject
{
	GENERATED_BODY()

public:
	UComposite();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Input", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_MediaInputTexture : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Soft Mask", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_EnableSoftMask : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Soft Mask", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_SoftMaskScreenPercentage : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Keyer", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_MediaInputKeyer : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_EnableMediaShadows : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ShadowsOffset : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ShadowsBlackLevel : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ShadowsWhiteLevel : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ShadowsGamma : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Shadows", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ShadowsTint : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planar Reflection on Mediar", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_EnablePlanarReflection : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planar Reflection on Media", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_PlanarReflectionColor : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planar Reflection on Media", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_PlanarReflectionDistortionIntensity : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planar Reflection on Media", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_PlanarReflectionDistortionOffset : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planar Reflection on Media", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_PlanarReflectionScreenPercentage : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Integration", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_BrightnessMaskGamma : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media Integration", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_ApplyInverseToneCurve : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_MediaBlend : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_OutputRgbEncoding : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Output", meta = (AllowPrivateAccess = "true", InlineEditConditionToggle))
	uint8 bOverride_OutputAlpha : 1;

private:
	UPROPERTY()
	UComposite* CompositeClassDefaults;

	/** The composite asset from which to get the default values. */
	UPROPERTY(Category = "Parent", EditAnywhere)
	UComposite* ParentComposite;

	/** The texture containing the media being composited into the scene. */
	UPROPERTY(Category = "Media Input", EditAnywhere, meta = (EditCondition = "bOverride_MediaInputTexture"))
	UTexture* MediaInputTexture;

	/** The actors showing up in the planar reflection. Try to keep this amount to a minimum. */
	UPROPERTY(Category = "Media Soft Mask", EditAnywhere, meta = (EditCondition = "bOverride_EnableSoftMask"))
	bool bEnableSoftMask;

	/** The actors showing up in the planar reflection. Try to keep this amount to a minimum. */
	UPROPERTY(Category = "Media Soft Mask", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "100.0", EditCondition = "bOverride_SoftMaskScreenPercentage"))
	float SoftMaskScreenPercentage;

	/** The Composite Keyer used on the media input. */
	UPROPERTY(Category = "Media Keyer", EditAnywhere, Instanced, Export, meta = (EditCondition = "bOverride_MediaInputKeyer"))
	UCompositeKeyer* MediaInputKeyer;

	/** Are shadows over media enabled? */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (EditCondition = "bOverride_EnableMediaShadows"))
	bool bEnableMediaShadows;

	/** Value is added to the shadows. */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (UIMin = "-1", UIMax = "1", EditCondition = "bOverride_ShadowsOffset"))
	float ShadowsOffset;

	/** Remap the black value of the shadow map. */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (UIMin = "-2", UIMax = "2", EditCondition = "bOverride_ShadowsBlackLevel"))
	float ShadowsBlackLevel;

	/** Remap the white value of the shadow map. */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (UIMin = "-2", UIMax = "2", EditCondition = "bOverride_ShadowsWhiteLevel"))
	float ShadowsWhiteLevel;

	/** Change the gamma of the shadow map. */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (UIMin = "0", UIMax = "2", EditCondition = "bOverride_ShadowsGamma"))
	float ShadowsGamma;

	/** This color is added to the media where the shadows are visible. */
	UPROPERTY(Category = "Media Shadows", EditAnywhere, meta = (UIMin = "-2", UIMax = "2", EditCondition = "bOverride_ShadowsTint", HideAlphaChannel))
	FLinearColor ShadowsTint;
	
	/** Is planar reflection enabled. Only enable if you need reflections from actors on top of the media. Only shows actors from the show only list. If no list is provided the planar reflection is still disabled. */
	UPROPERTY(Category = "Planar Reflection on Media", EditAnywhere, meta = (EditCondition = "bOverride_EnablePlanarReflection"))
	bool bEnablePlanarReflection;

	/** Planar Reflection Color. */
	UPROPERTY(Category = "Planar Reflection on Media", EditAnywhere, meta = (EditCondition = "bOverride_PlanarReflectionColor"))
	FLinearColor PlanarReflectionColor;

	/** Nonsensical planar reflection distortion media normal intensity. */
	UPROPERTY(Category = "Planar Reflection on Media", EditAnywhere, meta = (AllowPrivateAccess = "true", UIMin = "-1", UIMax = "1", EditCondition = "bOverride_PlanarReflectionDistortionIntensity"))
	float PlanarReflectionDistortionIntensity;

	/** Nonsensical planar reflection distortion media uv offset. */
	UPROPERTY(Category = "Planar Reflection on Media", EditAnywhere, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", EditCondition = "bOverride_PlanarReflectionDistortionOffset"))
	float PlanarReflectionDistortionOffset;

	/** The actors showing up in the planar reflection. Try to keep this amount to a minimum. */
	UPROPERTY(Category = "Planar Reflection on Media", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "100.0", EditCondition = "bOverride_PlanarReflectionScreenPercentage"))
	float PlanarReflectionScreenPercentage;

	UPROPERTY(Category = "Color Grading", EditInstanceOnly, Export, Instanced, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = "true"))
	UCompositeColorGrade* CompositeColorGrade;
	
	/** How is the media blended with the world. */
	UPROPERTY(Category = "Media Integration", EditAnywhere, meta = (EditCondition = "bOverride_MediaBlend"))
	EMediaBlend MediaBlend;

	/**
	When blending after the tone curve the shader will still try to mimic correct blending of bright values as close as possible by creating a brightness mask.
	This gamma value controls the brightness mask before it is multiplied with the media. lower values might lead to brighter edges while higher values may cause dark edges.
	*/
	UPROPERTY(Category = "Media Integration", EditAnywhere, AdvancedDisplay, meta = (ClampMin = "0.01", ClampMax = "100.0", UIMin = "0.1", UIMax = "2", EditCondition = "bOverride_BrightnessMaskGamma"))
	float BrightnessMaskGamma;

	/** Should an inverse tone curve be applied to the incoming media so the blending can happen linear. */
	UPROPERTY(Category = "Media Integration", EditAnywhere, AdvancedDisplay, meta = (EditCondition = "bOverride_ApplyInverseToneCurve"))
	bool bApplyInverseToneCurve;

	// The applied gamma curve.
	UPROPERTY(Category = "Output", EditAnywhere, meta = (EditCondition = "bOverride_OutputRgbEncoding"))
	EOutputRgbEncoding OutputRgbEncoding;

	/** Invert the alpha channel coming out of the engine. */
	UPROPERTY(Category = "Output", EditAnywhere, meta = (EditCondition = "bOverride_OutputAlpha"))
	EOutputAlpha OutputAlpha;

public:
	UFUNCTION(Category = "Composite", BlueprintPure)
	UComposite* GetParentComposite() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetParentComposite(UComposite* NewParentComposite);

	UFUNCTION(Category="Composite", BlueprintPure)
	UTexture* GetMediaInputTexture() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetMediaInputTexture(UTexture* NewMediaInputTexture);

	UFUNCTION(Category = "Composite", BlueprintPure)
	bool GetEnableSoftMask() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetEnableSoftMask(bool bNewEnableSoftMask);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetSoftMaskScreenPercentage() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetSoftMaskScreenPercentage(float NewSoftMaskScreenPercentage);

	UFUNCTION(Category = "Composite", BlueprintPure)
	UCompositeKeyer* GetMediaInputKeyer() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetMediaInputKeyer(UCompositeKeyer* NewMediaInputKeyer);

	UFUNCTION(Category = "Composite", BlueprintPure)
	bool GetEnableMediaShadows() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetEnableMediaShadows(bool bNewEnableMediaShadows);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetShadowsOffset() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetShadowsOffset(float NewShadowsOffset);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetShadowsBlackLevel() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetShadowsBlackLevel(float NewShadowsBlackLevel);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetShadowsWhiteLevel() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetShadowsWhiteLevel(float NewShadowsWhiteLevel);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetShadowsGamma() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetShadowsGamma(float NewShadowsGamma);

	UFUNCTION(Category = "Composite", BlueprintPure)
	FLinearColor GetShadowsTint() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetShadowsTint(FLinearColor NewShadowsTint);

	UFUNCTION(Category = "Composite", BlueprintPure)
	bool GetEnablePlanarReflection() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetEnablePlanarReflection(bool bNewEnablePlanarReflection);

	UFUNCTION(Category = "Composite", BlueprintPure)
	FLinearColor GetPlanarReflectionColor() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetPlanarReflectionColor(FLinearColor NewPlanarReflectionColor);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetPlanarReflectionDistortionIntensity() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetPlanarReflectionDistortionIntensity(float NewPlanarReflectionDistortionIntensity);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetPlanarReflectionDistortionOffset() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetPlanarReflectionDistortionOffset(float NewPlanarReflectionDistortionOffset);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetPlanarReflectionScreenPercentage() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetPlanarReflectionScreenPercentage(float NewPlanarReflectionScreenPercentage);
	
	UFUNCTION(Category = "Composite")
	UCompositeColorGrade* GetCompositeColorGrade() const;

	UFUNCTION(Category = "Composite", BlueprintPure)
	EMediaBlend GetMediaBlend() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetMediaBlend(EMediaBlend bNewMediaBlend);

	UFUNCTION(Category = "Composite", BlueprintPure)
	float GetBrightnessMaskGamma() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetBrightnessMaskGamma(float NewBrightnessMaskGamma);

	UFUNCTION(Category = "Composite", BlueprintPure)
	bool GetApplyInverseToneCurve() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetApplyInverseToneCurve(bool bNewApplyInverseToneCurve);

	UFUNCTION(Category = "Composite", BlueprintPure)
	EOutputRgbEncoding GetOutputRgbEncoding() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetOutputRgbEncoding(EOutputRgbEncoding NewOutputRgbEncoding);

	UFUNCTION(Category = "Composite", BlueprintPure)
	EOutputAlpha GetOutputAlpha() const;

	UFUNCTION(Category = "Composite", BlueprintCallable)
	void SetOutputAlpha(EOutputAlpha NewOutputAlpha);

	/** Is this Composite a descendant of the supplied Composite.*/
	UFUNCTION(Category = "Composite", BlueprintCallable)
	bool IsThisCompositeAnAncestorOf(UComposite* PossibleDescendant) const;

	/** Is the Keyer on this Composite not null and enabled.*/
	UFUNCTION(Category = "Composite", BlueprintPure)
	bool IsKeyerEnabled() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
};
