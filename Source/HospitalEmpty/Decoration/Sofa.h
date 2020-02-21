#pragma once

#include "CoreMinimal.h"
#include "RTSMode/Public/DecorationObj/DecorationBase.h"
#include "Sofa.generated.h"

/*
	第一个摆件，沙发
*/
UCLASS(BlueprintType, Blueprintable)
class ASofa : public ADecorationBase
{
	GENERATED_BODY()

public:
	ASofa(FString InActorName) : Super(InActorName) {};

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	ASofa(const FObjectInitializer& ObjectInitializer);

	/* virtual */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay();

	virtual void StartTouch(FVector TouchLocation) override {};
	virtual void TouchHold(FVector TouchLocation) override {};
	virtual void TouchEnd(FVector TouchLocation) override {};
	
	virtual void Hover(FVector TouchLocation) override {};
	/* virtual end */

protected:


};

