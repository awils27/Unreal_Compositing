// Copyright Epic Games, Inc. All Rights Reserved.

#include "DetailCustomizations/CompositorWorldSettingsDetails.h"
#include "Subsystems/CompositorSubsystem.h"
#include "Objects/CompositeWorldData.h"

#include "GameFramework/WorldSettings.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "FCompositorWorldSettingsDetails"

PRAGMA_DISABLE_OPTIMIZATION

TSharedRef<IDetailCustomization> FCompositorWorldSettingsDetails::MakeInstance()
{
	return MakeShareable(new FCompositorWorldSettingsDetails);
}

void FCompositorWorldSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	UObject* FirstObjectBeingCustomized = nullptr;

	if (ObjectsBeingCustomized.IsValidIndex(0))
	{
		FirstObjectBeingCustomized = ObjectsBeingCustomized[0].Get();
	}

	if (FirstObjectBeingCustomized)
	{
		UWorld* World = FirstObjectBeingCustomized->GetWorld();
		if (IsValid(World))
		{
			UCompositorSubsystem* CompositorSubsystem = World->GetSubsystem<UCompositorSubsystem>();
			if (IsValid(CompositorSubsystem))
			{
				CompositorSubsystem->OnCompositeWorldDataAdded.Clear();
				UCompositeWorldData* CompositeWorldData = CompositorSubsystem->GetCompositeWorldData();
				if (CompositeWorldData)
				{
					TArray<UObject*> CompositeWorldDataAsArray = { CompositeWorldData };
					FAddPropertyParams AddPropertyParams;
					AddPropertyParams.CreateCategoryNodes(false);
					AddPropertyParams.UniqueId(CompositeWorldData->GetFName());
					IDetailCategoryBuilder& CompositorCategory = DetailBuilder.EditCategory("Compositor");

					if (IDetailPropertyRow* CompositeWorldDataRow = CompositorCategory.AddExternalObjects(CompositeWorldDataAsArray, EPropertyLocation::Default, AddPropertyParams))
					{
						CompositeWorldDataRow->DisplayName(LOCTEXT("CompositeWorldData", "Composite World Data"));
						CompositeWorldDataRow->ToolTip(LOCTEXT("CompositeWorldDataToolTip", "Composite data for this world."));
						
						CompositeWorldDataRow->CustomWidget()
							.NameContent()
							[
								SNew(SBox)
								.Padding(2.0f)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CompositeWorldData", "Composite World Data"))
									.Font(IDetailLayoutBuilder::GetDetailFont())
								]
							];
					}
				}

				CompositorSubsystem->OnCompositeWorldDataAddedRaw.RemoveAll(this);
				CompositorSubsystem->OnCompositeWorldDataAddedRaw.AddSP(this, &FCompositorWorldSettingsDetails::ForceRefreshDetails);

				CompositorSubsystem->OnCompositeWorldDataRemovedRaw.RemoveAll(this);
				CompositorSubsystem->OnCompositeWorldDataRemovedRaw.AddSP(this, &FCompositorWorldSettingsDetails::ForceRefreshDetails);
			}
		}
	}
}

void FCompositorWorldSettingsDetails::CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder)
{
	DetailBuilderWeakPtr = DetailBuilder;
	CustomizeDetails(*DetailBuilder);
}

void FCompositorWorldSettingsDetails::ForceRefreshDetails()
{
	IDetailLayoutBuilder* DetailLayoutBuilder = DetailBuilderWeakPtr.Pin().Get();
	if (DetailLayoutBuilder)
	{
		DetailLayoutBuilder->ForceRefreshDetails();
	}
}

PRAGMA_ENABLE_OPTIMIZATION

#undef LOCTEXT_NAMESPACE