// Copyright Epic Games, Inc. All Rights Reserved.

#include "Assets/CompositeKeyerFromAsset.h"
#include "Subsystems/CompositorSubsystem.h"

void UCompositeKeyerFromAsset::InitializeCompositeKeyer(UCompositorSubsystem* CompositorSubsystem)
{
	if (IsValid(CompositeKeyerAsset))
	{
		return CompositeKeyerAsset->InitializeCompositeKeyer(CompositorSubsystem);
	}
}

void UCompositeKeyerFromAsset::UpdateCompositeKeyer(UCompositorSubsystem* CompositorSubsystem)
{
	if (IsValid(CompositeKeyerAsset))
	{
		return CompositeKeyerAsset->UpdateCompositeKeyer(CompositorSubsystem);
	}
}

bool UCompositeKeyerFromAsset::GetIsKeyerEnabled()
{
	if (IsValid(CompositeKeyerAsset) && Super::GetIsKeyerEnabled())
	{
		return CompositeKeyerAsset->GetIsKeyerEnabled();
	}
	return false;
}

void UCompositeKeyerFromAsset::SetCompositeKeyerAsset(UCompositeKeyer* NewCompositeKeyerAsset)
{
	CompositeKeyerAsset = NewCompositeKeyerAsset;
}

UMaterialInterface* UCompositeKeyerFromAsset::GetCompositeKeyerMaterial_Implementation() const
{
	if (IsValid(CompositeKeyerAsset))
	{
		return CompositeKeyerAsset->GetCompositeKeyerMaterial();
	}

	return nullptr;
}

UMaterialInstanceDynamic* UCompositeKeyerFromAsset::GetCompositeKeyerMID()
{
	if (IsValid(CompositeKeyerAsset))
	{
		return CompositeKeyerAsset->GetCompositeKeyerMID();
	}

	return nullptr;
}
