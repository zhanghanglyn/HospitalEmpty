#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameBase/GridMgr/GridSystemMgrBase.h"
#include "GameBase/DecorationObj/DecorationBase.h"
#include "Serialize/SaveableActorInterface.h"
#include "DecorationGridMgrComponent.generated.h"

/*
	家具格子的管理类，管理类只与管理类进行交互处理
	家具的格子，必须要与地面格子同步位置。
	会根据当前的位置与传入的GroundGridMgr，去进行计算当前家具的格子信息
*/
UCLASS()
class UDecorationGridMgrComponent : public UActorComponent , public ISaveableActorInterface
{
	GENERATED_BODY()

public:
	UDecorationGridMgrComponent() {};
	UDecorationGridMgrComponent(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {};

	virtual void Serialize(FArchive& Ar) override;
	friend FArchive& operator<<(FArchive& Ar, UDecorationGridMgrComponent& SaveRef);

public:
	/* 设置自身关联的家具 */
	void SetDecorationActor(class ADecorationBase* InDecorationActor)
	{
		DecorationActor = InDecorationActor;
	};

	void InitGridMgr(FDecorationConfig ParamData,class ADecorationBase* InDecorationActor);

	/* 设置与之关联的地面GridMgr */
	void SetGroundGridMgr(class UGroundGridMgrComponent* InGroundGridMgr);

	/* 创建时根据当前地面更新格子数据，并且让家具贴合到最近的格子中 */
	//void CreateInitGrid();

	/* 拖动物体时的移动,根据传入的坐标，会自动对齐到最近的格子 */
	void MoveTo(FVector InLocation);

	/* 获取地面Grid数据 */
	TArray< FNestedArray > GetGridList()
	{
		return GridDataList;
	};
protected:
	/* 根据左上角格子数据，更新自身格子信息 */
	void UpdateGridInfo(FGridData TopRightGridData);

protected:
	//格子宽高
	UPROPERTY()
	FVector2D GridRowColumn;

	//格子List
	UPROPERTY()
	TArray< FNestedArray > GridDataList;

protected:
	/* 自身管理的家具Actor */
	UPROPERTY()
	class ADecorationBase* DecorationActor;

	/* 放入其中的地面的总GridMgr */
	UPROPERTY()
	class UGroundGridMgrComponent* GroundGridMgr;

};

