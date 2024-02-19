// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CompositeFactory.generated.h"

/**
 * 
 */
UCLASS()
class COMPOSITOREDITOR_API UCompositeFactory : public UFactory
{
	GENERATED_BODY()


public:
	UCompositeFactory();

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};
