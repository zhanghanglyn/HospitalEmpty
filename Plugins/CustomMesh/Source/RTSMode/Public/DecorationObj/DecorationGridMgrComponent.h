#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridSystemMgrBase.h"
#include "DecorationGridMgrComponent.generated.h"

/*
	家具格子的管理类，管理类只与管理类进行交互处理
	家具的格子，必须要与地面格子同步位置。
	会根据当前的位置与传入的GroundGridMgr，去进行计算当前家具的格子信息
*/
UCLASS()
class UDecorationGridMgrComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDecorationGridMgrComponent() {};
	UDecorationGridMgrComponent(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {};


public:
	void InitGrid() {};

protected:
	//格子List
	UPROPERTY()
	TArray< FNestedArray > GridDataList;
};

