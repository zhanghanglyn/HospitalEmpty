#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DtSceneComponentBase.generated.h"

/*
	家具所有的显示，都会加载到该组件中
*/
UCLASS(ClassGroup = (Decoration), BlueprintType, meta = (BlueprintSpawnableComponent, IgnoreCategoryKeywordsInSubclasses, ShortTooltip = "家具的显示相关组件"))
class UDtSceneComponentBase : public USceneComponent
{
	GENERATED_BODY()

public:
	UDtSceneComponentBase(FString InActorName);

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	UDtSceneComponentBase(const FObjectInitializer& ObjectInitializer){
		PrimaryComponentTick.bCanEverTick = false;
	};

	/* virtual */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override {};
	/* virtual end */

protected:


};

