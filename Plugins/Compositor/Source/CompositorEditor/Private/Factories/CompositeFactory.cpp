// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/CompositeFactory.h"

#include "Assets/Composite.h"

UCompositeFactory::UCompositeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UComposite::StaticClass();
}

UObject* UCompositeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UComposite* Composite = NewObject<UComposite>(InParent, Class, Name, Flags);
	return Composite;
}
