// Copyright Epic Games, Inc. All Rights Reserved.

#include "Assets/Composite.h"

#include "MediaTexture.h"
#include "Subsystems/CompositorSubsystem.h"
#include "Assets/CompositeKeyer.h"
#include "Objects/CompositeColorGrade.h"
#include "Components/CompositePlanarReflectionComponent.h"

UComposite::UComposite()
{
    bEnableMediaShadows = true;
    ShadowsBlackLevel = 0.F;
    ShadowsWhiteLevel = 1.F;
    ShadowsGamma = 1.F;
    ShadowsTint = FLinearColor(0.F, 0.F, 0.F, 0.F);

    SoftMaskScreenPercentage = 25.F;
    PlanarReflectionColor = FLinearColor(1.F, 1.F, 1.F, 0.F);
    PlanarReflectionScreenPercentage = 50.F;
    MediaBlend = EMediaBlend::PostToneCurve;
    BrightnessMaskGamma = 1.F;
    bApplyInverseToneCurve = true;
    bEnableSoftMask = true;

    OutputRgbEncoding = EOutputRgbEncoding::Srgb;
    OutputAlpha = EOutputAlpha::Opacity;

    CompositeClassDefaults = Cast<UComposite>(UComposite::StaticClass()->GetDefaultObject(true));

    CompositeColorGrade = CreateDefaultSubobject<UCompositeColorGrade>(TEXT("CompositeColorGrade"), /* bTransient = */false);
}

UComposite* UComposite::GetParentComposite() const
{
    return ParentComposite;
}

void UComposite::SetParentComposite(UComposite* NewParentComposite)
{
    ParentComposite = NewParentComposite;
}

UTexture* UComposite::GetMediaInputTexture() const
{
    if (bOverride_MediaInputTexture)
    {
        return MediaInputTexture;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetMediaInputTexture();
    }
    
    return nullptr;
}

void UComposite::SetMediaInputTexture(UTexture* NewMediaInputTexture)
{
    MediaInputTexture = NewMediaInputTexture;
}

bool UComposite::GetEnableSoftMask() const
{
    if (bOverride_EnableSoftMask)
    {
        return bEnableSoftMask;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetEnableSoftMask();
    }

    return CompositeClassDefaults->bEnableSoftMask;

}

void UComposite::SetEnableSoftMask(bool bNewEnableSoftMask)
{
    bEnableSoftMask = bNewEnableSoftMask;
}

float UComposite::GetSoftMaskScreenPercentage() const
{
    if (bOverride_SoftMaskScreenPercentage)
    {
        return SoftMaskScreenPercentage;
    }
    
    if (ParentComposite)
    {
        return ParentComposite->GetSoftMaskScreenPercentage();
    }

    return CompositeClassDefaults->SoftMaskScreenPercentage;
}

void UComposite::SetSoftMaskScreenPercentage(float NewSoftMaskScreenPercentage)
{
    SoftMaskScreenPercentage = NewSoftMaskScreenPercentage;
}

UCompositeKeyer* UComposite::GetMediaInputKeyer() const
{
    if (bOverride_MediaInputKeyer)
    {
        return MediaInputKeyer;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetMediaInputKeyer();
    }

    return nullptr;
}

void UComposite::SetMediaInputKeyer(UCompositeKeyer* NewMediaInputCompositeKeyer)
{
    MediaInputKeyer = NewMediaInputCompositeKeyer;
}

bool UComposite::GetEnableMediaShadows() const
{
    if (bOverride_EnableMediaShadows)
    {
        return bEnableMediaShadows;
    }
    
    if (ParentComposite)
    {
        return ParentComposite->GetEnableMediaShadows();
    }

    return CompositeClassDefaults->bEnableMediaShadows;
}

void UComposite::SetEnableMediaShadows(bool bNewEnableMediaShadows)
{
    bEnableMediaShadows = bNewEnableMediaShadows;
}

float UComposite::GetShadowsOffset() const
{
    if (bOverride_ShadowsOffset)
    {
        return ShadowsOffset;
    }
    
    if (ParentComposite)
    {
        return ParentComposite->GetShadowsOffset();
    }

    return CompositeClassDefaults->ShadowsOffset;
}

void UComposite::SetShadowsOffset(float NewShadowsOffset)
{
    ShadowsOffset = NewShadowsOffset;
}

float UComposite::GetShadowsBlackLevel() const
{
    if (bOverride_ShadowsBlackLevel)
    {
        return ShadowsBlackLevel;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetShadowsBlackLevel();
    }

    return CompositeClassDefaults->ShadowsBlackLevel;
}

void UComposite::SetShadowsBlackLevel(float NewShadowsBlackLevel)
{
    ShadowsBlackLevel = NewShadowsBlackLevel;
}

float UComposite::GetShadowsWhiteLevel() const
{
    if (bOverride_ShadowsWhiteLevel)
    {
        return ShadowsWhiteLevel;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetShadowsWhiteLevel();
    }

    return CompositeClassDefaults->ShadowsWhiteLevel;
}

void UComposite::SetShadowsWhiteLevel(float NewShadowsWhiteLevel)
{
    ShadowsWhiteLevel = NewShadowsWhiteLevel;
}

float UComposite::GetShadowsGamma() const
{
    if (bOverride_ShadowsGamma)
    {
        return ShadowsGamma;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetShadowsGamma();
    }

    return CompositeClassDefaults->ShadowsGamma;
}

void UComposite::SetShadowsGamma(float NewShadowsGamma)
{
    ShadowsGamma = NewShadowsGamma;
}

FLinearColor UComposite::GetShadowsTint() const
{
    if (bOverride_ShadowsTint)
    {
        return ShadowsTint;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetShadowsTint();
    }
        
    return CompositeClassDefaults->ShadowsTint;
}

void UComposite::SetShadowsTint(FLinearColor NewShadowsTint)
{
    ShadowsTint = NewShadowsTint;
}

bool UComposite::GetEnablePlanarReflection() const
{
    if (bOverride_EnablePlanarReflection)
    {
        return bEnablePlanarReflection;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetEnablePlanarReflection();
    }

    return CompositeClassDefaults->bEnablePlanarReflection;
}

void UComposite::SetEnablePlanarReflection(bool bNewEnablePlanarReflection)
{
    bEnablePlanarReflection = bNewEnablePlanarReflection;
}

FLinearColor UComposite::GetPlanarReflectionColor() const
{
    if (bOverride_PlanarReflectionColor)
    {
        return PlanarReflectionColor;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetPlanarReflectionColor();
    }

    return CompositeClassDefaults->PlanarReflectionColor;
}

void UComposite::SetPlanarReflectionColor(FLinearColor NewPlanarReflectionColor)
{
    PlanarReflectionColor = NewPlanarReflectionColor;
}

float UComposite::GetPlanarReflectionDistortionIntensity() const
{
    if (bOverride_PlanarReflectionDistortionIntensity)
    {
        return PlanarReflectionDistortionIntensity;
    }
    
    if (ParentComposite)
    {
        return ParentComposite->GetPlanarReflectionDistortionIntensity();
    }

    return CompositeClassDefaults->PlanarReflectionDistortionIntensity;
}

void UComposite::SetPlanarReflectionDistortionIntensity(float NewPlanarReflectionDistortionIntensity)
{
    PlanarReflectionDistortionIntensity = NewPlanarReflectionDistortionIntensity;
}

float UComposite::GetPlanarReflectionDistortionOffset() const
{
    if (bOverride_PlanarReflectionDistortionOffset)
    {
        return PlanarReflectionDistortionOffset;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetPlanarReflectionDistortionOffset();
    }

    return CompositeClassDefaults->PlanarReflectionDistortionOffset;
}

void UComposite::SetPlanarReflectionDistortionOffset(float NewPlanarReflectionDistortionOffset)
{
    PlanarReflectionDistortionOffset = NewPlanarReflectionDistortionOffset;
}

float UComposite::GetPlanarReflectionScreenPercentage() const
{
    if (bOverride_PlanarReflectionScreenPercentage)
    {
        return PlanarReflectionScreenPercentage;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetPlanarReflectionScreenPercentage();
    }

    return CompositeClassDefaults->PlanarReflectionScreenPercentage;
}

void UComposite::SetPlanarReflectionScreenPercentage(float NewPlanarReflectionScreenPercentage)
{
    PlanarReflectionScreenPercentage = NewPlanarReflectionScreenPercentage;
}

UCompositeColorGrade* UComposite::GetCompositeColorGrade() const
{
    return CompositeColorGrade;
}

EMediaBlend UComposite::GetMediaBlend() const
{
    if (bOverride_MediaBlend)
    {
        return MediaBlend;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetMediaBlend();
    }

    return CompositeClassDefaults->MediaBlend;
}

void UComposite::SetMediaBlend(EMediaBlend bNewMediaBlend)
{
    MediaBlend = bNewMediaBlend;
}

float UComposite::GetBrightnessMaskGamma() const
{
    if (bOverride_BrightnessMaskGamma)
    {
        return BrightnessMaskGamma;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetBrightnessMaskGamma();
    }

    return CompositeClassDefaults->BrightnessMaskGamma;
}

void UComposite::SetBrightnessMaskGamma(float NewBrightnessMaskGamma)
{
    BrightnessMaskGamma = NewBrightnessMaskGamma;
}

bool UComposite::GetApplyInverseToneCurve() const
{
    if (bOverride_ApplyInverseToneCurve)
    {
        return bApplyInverseToneCurve;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetApplyInverseToneCurve();
    }

    return CompositeClassDefaults->bApplyInverseToneCurve;
}

void UComposite::SetApplyInverseToneCurve(bool bNewApplyInverseToneCurve)
{
    bApplyInverseToneCurve = bNewApplyInverseToneCurve;
}

EOutputRgbEncoding UComposite::GetOutputRgbEncoding() const
{
    if (bOverride_OutputRgbEncoding)
    {
        return OutputRgbEncoding;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetOutputRgbEncoding();
    }

    return CompositeClassDefaults->OutputRgbEncoding;
}

void UComposite::SetOutputRgbEncoding(EOutputRgbEncoding NewOutputRgbEncoding)
{
    OutputRgbEncoding = NewOutputRgbEncoding;
}

EOutputAlpha UComposite::GetOutputAlpha() const
{
    if (bOverride_OutputAlpha)
    {
        return OutputAlpha;
    }

    if (ParentComposite)
    {
        return ParentComposite->GetOutputAlpha();
    }

    return CompositeClassDefaults->OutputAlpha;
}

void UComposite::SetOutputAlpha(EOutputAlpha NewOutputAlpha)
{
    OutputAlpha = NewOutputAlpha;
}

bool UComposite::IsThisCompositeAnAncestorOf(UComposite* PossibleDescendant) const
{
    UComposite* CompositeToValidate = PossibleDescendant;
    while (CompositeToValidate != nullptr)
    {
        if (CompositeToValidate == this)
        {
            return true;
        }

        CompositeToValidate = CompositeToValidate->ParentComposite;
    }

    return false;
}

bool UComposite::IsKeyerEnabled() const
{
    UCompositeKeyer* CurrentKeyer = GetMediaInputKeyer();
    return CurrentKeyer != nullptr && CurrentKeyer->GetIsKeyerEnabled();
}

#if WITH_EDITOR
void UComposite::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == NAME_None)
    {
        return;
    }

    // Make sure the parent composite does not contain the child composite.
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UComposite, ParentComposite))
    {
        if (IsThisCompositeAnAncestorOf(ParentComposite))
        {
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::Format(FText::FromString("Cannot set {0} as a parent as it is already a descendant of this Composite."), FText::FromString(ParentComposite->GetName())));

            ParentComposite = nullptr;
        }
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UComposite, MediaInputTexture))
    {
        if (UMediaTexture* MediaTexture = Cast<UMediaTexture>(MediaInputTexture))
        {
            if (!MediaTexture->NewStyleOutput)
            {
                MediaTexture->NewStyleOutput = true;                
#if WITH_EDITOR
                MediaTexture->Modify();
                FMessageDialog::Open(
                    EAppMsgType::Ok,
                    FText::Format(FText::FromString("The Compositor has set Media Texture {0} New Output Style to True because False is not supported. Please save the asset."), FText::FromString(MediaInputTexture->GetName())));      
#endif          
            }
        }
    }
    
    UWorld* CurrentWorld = GetWorld();

    if (GEditor)
    {
        FWorldContext* WorldContext = GEditor->GetPIEWorldContext();
        if (WorldContext)
        {
            CurrentWorld = WorldContext->World();
        }
        else
        {
            CurrentWorld = GEditor->GetEditorWorldContext(false).World();
        }
    }

    if (CurrentWorld != nullptr)
    {
        UCompositorSubsystem* CompositorSubsystem = CurrentWorld->GetSubsystem<UCompositorSubsystem>();
        if (CompositorSubsystem != nullptr)
        {
            UComposite* WorldComposite = CompositorSubsystem->GetWorldComposite();
            if (WorldComposite != nullptr && (WorldComposite == this || IsThisCompositeAnAncestorOf(WorldComposite)))
            {                
                CompositorSubsystem->OnCompositeUpdate_Internal();
            }
        }
    }
}

bool UComposite::CanEditChange(const FProperty* InProperty) const
{
    if (InProperty)
    {
        const FString PropertyName = InProperty->GetName();

        if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, SoftMaskScreenPercentage))
        {
            return GetEnableSoftMask();
        }

        if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, ShadowsOffset)
            || PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, ShadowsBlackLevel)
            || PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, ShadowsWhiteLevel)
            )
        {
            return GetEnableMediaShadows();
        }

        if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, PlanarReflectionColor)
            || PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, PlanarReflectionDistortionIntensity)
            || PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, PlanarReflectionDistortionOffset)
            || PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, PlanarReflectionScreenPercentage)
            )
        {
            return GetEnablePlanarReflection();
        }

        if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, BrightnessMaskGamma))
        {
            return  GetMediaBlend() == EMediaBlend::PostToneCurve;
        }

        if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UComposite, bApplyInverseToneCurve))
        {
            return  GetMediaBlend() == EMediaBlend::PreToneCurve;
        }
    }

    return Super::CanEditChange(InProperty);    
}
#endif
