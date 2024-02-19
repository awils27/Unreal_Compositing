// Copyright Epic Games, Inc. All Rights Reserved.


#include "Assets/CompositeKeyer.h"
#include "Subsystems/CompositorSubsystem.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

UCompositeKeyer::UCompositeKeyer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsCompositeKeyerEnabled = true;
	MediaInputKeyedRenderTarget = Cast<UTextureRenderTarget2D>(FSoftObjectPath(TEXT("/Compositor/CompositeKeyer/RT_MediaInputKeyed.RT_MediaInputKeyed")).TryLoad());
}

void UCompositeKeyer::InitializeCompositeKeyer(UCompositorSubsystem* CompositorSubsystem)
{
	if (IsValid(GetCompositeKeyerMaterial()))
	{
		if (CompositeKeyerMID == nullptr || CompositeKeyerMID->Parent != GetCompositeKeyerMaterial())
		{
			CompositeKeyerMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(nullptr, GetCompositeKeyerMaterial(), FName("CompositeKeyer"), EMIDCreationFlags::Transient);
		}

		if (GetCompositeKeyerMID())
		{
			ReceiveInitializeCompositeKeyer();

			BoolParametersForMID.Empty();
			FloatParametersForMID.Empty();
			LinearColorParametersForMID.Empty();

			for (TFieldIterator<FProperty> PropertyIterator(GetClass(), EFieldIteratorFlags::IncludeSuper); PropertyIterator; ++PropertyIterator)
			{
				FProperty* Property = *PropertyIterator;
				FName const PropertyName = Property->GetFName();
				FString PropertyNameAsString = PropertyName.ToString();

				FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property);
				if (BoolProperty)
				{
					const bool boolPropertyValue = BoolProperty->GetPropertyValue_InContainer(this);
					float currentParameterValueInMID;
					if (GetCompositeKeyerMID()->GetScalarParameterValue(PropertyName, currentParameterValueInMID, false))
					{
						int32 ParameterIndex;
						if (GetCompositeKeyerMID()->InitializeScalarParameterAndGetIndex(PropertyName, boolPropertyValue ? 1.F : 0.F, ParameterIndex))
						{
							BoolParametersForMID.Add(ParameterIndex, BoolProperty);
						}
					}
				}

				FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property);
				if (FloatProperty)
				{
					float* floatPropertyValue = FloatProperty->GetPropertyValuePtr_InContainer(this);
					float currentParameterValueInMID;
					if (GetCompositeKeyerMID()->GetScalarParameterValue(PropertyName, currentParameterValueInMID, false))
					{
						int32 ParameterIndex;
						if (GetCompositeKeyerMID()->InitializeScalarParameterAndGetIndex(PropertyName, *floatPropertyValue, ParameterIndex))
						{
							FloatParametersForMID.Add(ParameterIndex, floatPropertyValue);
						}
					}
				}

				FStructProperty* StructProperty = CastField<FStructProperty>(Property);
				if (StructProperty)
				{
					FLinearColor* LinearColor = StructProperty->ContainerPtrToValuePtr<FLinearColor>(this);
					if (LinearColor)
					{
						FLinearColor currentParameterValueInMID;
						if (GetCompositeKeyerMID()->GetVectorParameterValue(PropertyName, currentParameterValueInMID, false))
						{
							int32 ParameterIndex;
							if (GetCompositeKeyerMID()->InitializeVectorParameterAndGetIndex(PropertyName, *LinearColor, ParameterIndex))
							{
								LinearColorParametersForMID.Add(ParameterIndex, LinearColor);
							}
						}
					}
				}
			}

			UKismetRenderingLibrary::DrawMaterialToRenderTarget(CompositorSubsystem, MediaInputKeyedRenderTarget, GetCompositeKeyerMID());
		}
		else
		{
			UE_LOG(LogCompositor, Error, TEXT("Failed to create material instance dynamic from CompositeKeyer material."));
		}
	}
	else
	{
		UE_LOG(LogCompositor, Error, TEXT("Missing CompositeKeyer material."));
	}
}

void UCompositeKeyer::UpdateCompositeKeyer(UCompositorSubsystem* CompositorSubsystem)
{
	if (GetCompositeKeyerMID() && GetCompositeKeyerMID()->Parent == GetCompositeKeyerMaterial())
	{
		// Update all bool parameters.
		for (const TPair<int32, FBoolProperty*>& pair : BoolParametersForMID)
		{
			GetCompositeKeyerMID()->SetScalarParameterByIndex(pair.Key, pair.Value->GetPropertyValue_InContainer(this) ? 1.F : 0.F);
		}

		// Update all float parameters.
		for (const TPair<int32, float*>& pair : FloatParametersForMID)
		{
			GetCompositeKeyerMID()->SetScalarParameterByIndex(pair.Key, *pair.Value);
		}

		// Update all linear color parameters.
		for (const TPair<int32, FLinearColor*>& pair : LinearColorParametersForMID)
		{
			GetCompositeKeyerMID()->SetVectorParameterByIndex(pair.Key, *pair.Value);
		}
		
		ReceiveUpdateCompositeKeyer();

		UKismetRenderingLibrary::DrawMaterialToRenderTarget(CompositorSubsystem, MediaInputKeyedRenderTarget, GetCompositeKeyerMID());
	}
	else
	{
		InitializeCompositeKeyer(CompositorSubsystem);
		
		if (GetCompositeKeyerMID())
		{
			UE_LOG(LogCompositor, Warning, TEXT("Not updating CompositeKeyer because the material instance dynamic is null or the parent is not matching the keyer material, reinitializing..."));			
		}
	}
}

UMaterialInterface* UCompositeKeyer::GetCompositeKeyerMaterial_Implementation() const
{
	return nullptr;
}

UMaterialInstanceDynamic* UCompositeKeyer::GetCompositeKeyerMID()
{
	return CompositeKeyerMID;
}

bool UCompositeKeyer::GetIsKeyerEnabled()
{
	return IsCompositeKeyerEnabled;
}
