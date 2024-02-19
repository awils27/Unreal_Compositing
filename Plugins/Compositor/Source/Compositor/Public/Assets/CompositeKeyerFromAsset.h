// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CompositeKeyer.h"
#include "CompositeKeyerFromAsset.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, DisplayName="Asset Reference")
class COMPOSITOR_API UCompositeKeyerFromAsset : public UCompositeKeyer
{
	GENERATED_BODY()

public:
	void InitializeCompositeKeyer(UCompositorSubsystem* CompositorSubsystem) override;

	void UpdateCompositeKeyer(UCompositorSubsystem* CompositorSubsystem) override;

	bool GetIsKeyerEnabled() override;
	
	UFUNCTION(Category="Compositor|CompositeKeyer", BlueprintCallable)
	void SetCompositeKeyerAsset(UCompositeKeyer* NewCompositeKeyerAsset);

protected:
	UPROPERTY(Category = "Asset Reference", EditAnywhere, BlueprintReadOnly, AdvancedDisplay, meta = (AllowPrivateAccess = "true"))
	UCompositeKeyer* CompositeKeyerAsset;

	UMaterialInterface* GetCompositeKeyerMaterial_Implementation() const override;

	virtual UMaterialInstanceDynamic* GetCompositeKeyerMID() override;
};
