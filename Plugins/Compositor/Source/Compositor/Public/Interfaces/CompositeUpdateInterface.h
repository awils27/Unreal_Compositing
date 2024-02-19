// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "CompositeUpdateInterface.generated.h"

class UComposite;

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class COMPOSITOR_API UCompositeUpdateInterface : public UInterface
{
	GENERATED_BODY()
};

class COMPOSITOR_API ICompositeUpdateInterface
{
	GENERATED_BODY()

public:
	/** Called whenever a composite related to the world is updated. */
	virtual void OnCompositeUpdate(UComposite& InWorldComposite) = 0;
};