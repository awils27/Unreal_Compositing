// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CompositeKeyerFactory.generated.h"

class UCompositeKeyer;

/**
 * 
 */
UCLASS()
class COMPOSITOREDITOR_API UCompositeKeyerFactory : public UFactory
{
	GENERATED_BODY()

 public:
	UPROPERTY(EditAnywhere, Category="Compositor CompositeKeyer")
	TSubclassOf<UCompositeKeyer> CompositeKeyerParentClass;

    UCompositeKeyerFactory(const class FObjectInitializer& ObjectInitializer);

	virtual FText GetDisplayName() const override;
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

protected:
    virtual bool IsMacroFactory() const { return false; }	
};
