#pragma once

#include "CoreMinimal.h"
#include "GameBase/DecorationObj/DecorationBase.h"
#include "Wall.generated.h"

/*
	第一个摆件，沙发
*/
UCLASS(BlueprintType, Blueprintable)
class AWall : public ADecorationBase
{
	GENERATED_BODY()

public:
	AWall(FString InActorName) : Super(InActorName) {};

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	AWall(const FObjectInitializer& ObjectInitializer);

	/* virtual */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay();

	virtual void StartTouch(FVector TouchLocation) override {};
	virtual void TouchHold(FVector TouchLocation) override {};
	virtual void TouchEnd(FVector TouchLocation) override {};
	
	virtual void Hover(FVector TouchLocation) override {};
	/* virtual end */

	/* 根据传入的格子，拉长或创建墙体 */
	void ExpendWall(  );

protected:


};

