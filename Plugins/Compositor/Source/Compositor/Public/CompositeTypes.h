// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeTypes.generated.h"

UENUM()
enum class EOutputRgbEncoding : uint8
{
	/** 
	* No gamma curve is applied.
	* The output is a linear encoded image.
	*/
	Linear,

	/** 
	* A linear to sRGB curve is applied (~2.2).
	* The output is an sRGB encoded image.
	*/
	Srgb UMETA(DisplayName = "sRGB")
};

UENUM()
enum class EMediaBlend : uint8
{
	/** This won't blend in the media, so the composite meshes stay black.
	* The output is essentially a premultiplied RGB.
	* This can be used for external compositing.
	* Make sure to still supply a media import as an approximation for reflections.
	* Only use separate translucency, otherwise it won't show up in the alpha.
	*/
	None,

	/** 
	* The media will be blended before the tone curve.
	* Gives better color blending results but the media does change color based on the tone curve related settings.
	* Only works properly if Tone Curve Amount = 1 in the post process settings.
	*/
	PreToneCurve,

	/**
	* Media will be blended after the tone curve.
	* This can cause clipping artifacts which are reduced by a brightness mask.
	* Use the brightness mask gamma to tweak it to your liking.
	*/
	PostToneCurve,
};

UENUM()
enum class EOutputAlpha : uint8
{
	/** Black */
	Black,

	/** White */
	White,

	/**
	* White = Opaque, Black = Transparent.
	* Can be used when compositing externally using a pre-multipled alpha blend (AKA "Over" blend).
	* External Comp ~= (1-unreal.a) * your_media.rgb + unreal.rgb;
	*/
	Opacity,

	/**
	* The default value Unreal works with.
	* Black = Opaque, White = Transparent.
	* External Comp ~= unreal.a * your_media.rgb + unreal.rgb;
	*/
	InvertedOpacity
};

UENUM()
enum class ERenderSoftMaskType : uint8
{
	/** Render as opaque black geometry into the soft mask. */
	OpaqueBlack,

	/** Render as opaque white geometry into the soft mask. */
	OpaqueWhite,

	/** Render as opaque vertex color alpha geometry into the soft mask.	*/
	OpaqueVertexColorAlpha,

	/** Render as translucent vertex color alpha geometry into the soft mask.	*/
	TranslucentVertexColorAlpha
};
