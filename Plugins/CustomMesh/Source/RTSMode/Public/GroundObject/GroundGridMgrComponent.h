﻿#pragma once

#include "CoreMinimal.h"
#include "GroundUtil.h"
#include "Components/ActorComponent.h"
#include "GridSystemMgrBase.h"
//#include "NestedArray.h"
#include "GroundGridMgrComponent.generated.h"

/* 
	地板格子数据的管理类
	地板格子的起始点，就是地板Mesh的00点，与mesh的大小一一对应（但是生成时是使用的FLOOR()）
*/
UCLASS()
class UGroundGridMgrComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGroundGridMgrComponent();
	UGroundGridMgrComponent(const FObjectInitializer& ObjectInitializer);


	void InitGridData( FVector2D InWidthHeight , class AGroundObj* InGroundActor);
	//设置当前格子的左上角起始点在世界空间中的位置（和地面一致）
	void InitGridStartLocation(FVector InStartLocation) { StartLocation = InStartLocation; };

	//根据传入的点击位置获取点击到的格子 param: GridData 返回的当前位置的格子数据
	void GetTouchGrid( FVector TouchLocation, FGridData& GridData);

	//获取当前的横纵格子数量
	int32 GetGridRow() { return GridWidthNum; };
	int32 GetGridColumn() { return GridWidthNum; };

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

	/* 自身管理的地面Actor */
	UPROPERTY()
	class AGroundObj* GroundActor;
	
};