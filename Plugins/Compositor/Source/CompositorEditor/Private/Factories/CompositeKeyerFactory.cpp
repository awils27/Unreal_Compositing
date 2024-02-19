// Fill out your copyright notice in the Description page of Project Settings.


#include "Factories/CompositeKeyerFactory.h"

#include "Assets/CompositeKeyer.h"

#include "ClassViewerModule.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ClassViewerFilter.h"

class FCompositeKeyerClassFilter : public IClassViewerFilter
{
public:
	/** All children of these classes will be included unless filtered out by another setting. */
	TSet< const UClass* > AllowedChildrenOfClasses;

	TSet< const UClass* > DisallowedClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags) && InFilterFuncs->IfInClassesSet( DisallowedClasses , InClass ) == EFilterReturn::Failed
			&& InFilterFuncs->IfInChildOfClassesSet( AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) && InFilterFuncs->IfInClassesSet( DisallowedClasses , InUnloadedClassData ) == EFilterReturn::Failed
			&& InFilterFuncs->IfInChildOfClassesSet( AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

UCompositeKeyerFactory::UCompositeKeyerFactory(const class FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
    SupportedClass = UCompositeKeyer::StaticClass();
    bEditAfterNew = true;
    bCreateNew = true;
}

FText UCompositeKeyerFactory::GetDisplayName() const
{
	return FText::FromString("Composite Keyer");
}

bool UCompositeKeyerFactory::ConfigureProperties()
{
	// NULL the DataAssetClass so we can check for selection
	//StyleType = NULL;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FCompositeKeyerClassFilter> Filter = MakeShareable(new FCompositeKeyerClassFilter);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UCompositeKeyer::StaticClass());
	Filter->DisallowedClasses.Add(UCompositeKeyer::StaticClass());

	const FText TitleText = FText::FromString("Pick a Compositor CompositeKeyer"); // NSLOCTEXT("SlateWidgetStyleAssetFactory", "CreateSlateWidgetStyleAssetOptions", "Pick Slate Widget Style Class");
	UClass* ChosenClass = NULL;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UCompositeKeyer::StaticClass());
	
	if ( bPressedOk )
	{
		CompositeKeyerParentClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UCompositeKeyerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    check(Class->IsChildOf(UCompositeKeyer::StaticClass()));

    //return NewObject<UCompositeKeyer>(InParent, Class, Name, Flags|RF_Transactional, Context);

	//UCompositeKeyer* NewCompositeKeyer = NewObject<UCompositeKeyer>(InParent, Name, Flags);


	return NewObject<UCompositeKeyer>(InParent, CompositeKeyerParentClass, Name, Flags);
}
