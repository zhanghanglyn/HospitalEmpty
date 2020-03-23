#pragma once

#include "CoreMinimal.h"
#include "GameBase/RTSUtil/GroundUtil.h"
#include "Components/ActorComponent.h"
#include "GameBase/GridMgr/GridSystemMgrBase.h"
#include "Serialize/SaveableActorInterface.h"
#include "GroundGridMgrComponent.generated.h"

/* 
	地板格子数据的管理类
	地板格子的起始点，就是地板Mesh的00点，与mesh的大小一一对应（但是生成时是使用的FLOOR()）
*/
UCLASS()
class UGroundGridMgrComponent : public UActorComponent , public ISaveableActorInterface
{
	GENERATED_BODY()

public:
	UGroundGridMgrComponent();
	UGroundGridMgrComponent(const FObjectInitializer& ObjectInitializer);

	virtual void Serialize(FArchive& Ar) override;
	friend FArchive& operator<<(FArchive& Ar, UGroundGridMgrComponent& SaveRef);

	void InitGridData( FVector2D InWidthHeight , class AGroundObj* InGroundActor);
	//设置当前格子的左上角起始点在世界空间中的位置（和地面一致）
	void InitGridStartLocation(FVector InStartLocation) { StartLocation = InStartLocation; };

	//根据传入的点击位置获取点击到的格子 param: GridData 返回的当前位置的格子数据
	void GetTouchGrid( FVector TouchLocation, FGridData& GridData);

	//获取当前的横纵格子数量
	int32 GetGridRow() { return GridWidthNum; };
	int32 GetGridColumn() { return GridWidthNum; };

	/*
		更新阻挡格子信息，
		家具调用该接口，根据自身格子信息更新阻挡格子，但只有当点击后，才会将数据正式写入正式数据中
		该接口是通过查找当前地面上的所有家具，来更新
	*/
	void UpdateBlockGrid( class ADecorationBase* CurDecoration );//TArray< FNestedArray > BlockGridList);

	/* 点击保存当前编辑的物体 , 将阻挡格子上的数据写入正式数据 */
	bool SaveCurDecoration(class ADecorationBase* SaveDecoration);

	/* 判断当前家具的位置是否被阻挡 BlockGrid是否占用*/
	bool CheckGridsBeBlock(TArray< FNestedArray > DecorationGridList);

	/* 判断一个右上角点是否在当前格子地面上,修改为会把移动点变成右上角边缘 */
	bool CheckLocationInGround(FVector &InLocation) const;

	/* 判断一个左下角的点是否在格子地面上，如果不在，会重新设置左下角的坐标 */
	bool CheckLeftBUttomLocationInGround(FVector &LeftButtomLocation);

	/* 把一个Decoration加入自身 */
	void AddDecoration(class ADecorationBase* NewDecoration);

	/* 删除一个Decoration */
	void DeleteDecoration(class ADecorationBase* DelDecoration);

protected:
	/* 初始化遮挡格子 */
	void InitBlockGrid();
protected:

	UPROPERTY()
	int32 GridWidthNum = 0;	//横向格子数量

	UPROPERTY()
	int32 GridHeightNum = 0;//纵向格子数量

	UPROPERTY()
	FVector2D WidthHeight = FVector2D::ZeroVector;

	UPROPERTY()
	FVector StartLocation;	//当前格子管理的左上角起始点在世界空间中的位置（和地面一致）

	//UPROPERTY()
	//TArray<FGridData> GridDataList;	//格子list   20.2.5 废弃，换为二维的Grid数组

protected:
	//格子的二维数组NestArray
	UPROPERTY()
	TArray< FNestedArray > GridDataListNested;

	//用来单独存储被占用的格子，遍历时能减少大量计算
	UPROPERTY()
	TArray<FNestedArray> BlockGridDataList;

	/* 当前地面上的家具列表,正在操作中的家具不会计入列表中，但是会参与阻挡计算 */
	UPROPERTY()
	TArray<class ADecorationBase*> DecorationList;

	/* 自身管理的地面Actor */
	UPROPERTY()
	class AGroundObj* GroundActor;

	//当前地面是否有重复阻挡物体
	UPROPERTY()
	bool BeBlockDecorate = false;



};