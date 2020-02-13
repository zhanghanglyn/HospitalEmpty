#include "GroundGridMgrComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GroundObj.h"

UGroundGridMgrComponent::UGroundGridMgrComponent()
{

}

UGroundGridMgrComponent::UGroundGridMgrComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGroundGridMgrComponent::InitGridData(FVector2D InWidthHeight , class AGroundObj* InGroundActor)
{
	//初始化先清空所有List
	BlockGridDataList.Empty();
	GridDataListNested.Empty();

	GroundActor = InGroundActor;

	GridWidthNum = UKismetMathLibrary::FFloor(InWidthHeight.X / FGroundUtil::GroundGridWidth);	//一行有多少格子
	GridHeightNum = UKismetMathLibrary::FFloor(InWidthHeight.Y / FGroundUtil::GroundGridHeight);	

	WidthHeight = FVector2D(InWidthHeight);

	//初始化GridList
	for (int32 Row = 0; Row < GridWidthNum; ++Row)
	{
		for (int32 Column = 0; Column < GridHeightNum; ++Column)
		{
			FGridData NewGridData;
			NewGridData.SetGridId();

			/* 计算每一个格子的位置并记录 */
			FVector GridLocation = FVector::ZeroVector;
			GridLocation.Z = StartLocation.Z;
			GridLocation.X = StartLocation.X + Row * FGroundUtil::GroundGridWidth;
			GridLocation.Y = StartLocation.Y + Column * FGroundUtil::GroundGridHeight;
			NewGridData.StartLocation = GridLocation;
			NewGridData.GridRow = Row;
			NewGridData.GridColumn = Column;
			NewGridData.BeOccupy = false;	//是否被占用

			//GridDataList.Add(NewGridData);
			if (GridDataListNested.Num() <= Row)
			{
				FNestedArray tempArray;
				GridDataListNested.Add(tempArray);
			}
			GridDataListNested[Row].Array.Add( NewGridData);

			//初始化阻挡格子
			if (BlockGridDataList.Num() <= Row)
			{
				FNestedArray tempArray;
				BlockGridDataList.Add(tempArray);
			}
			BlockGridDataList[Row].Array.Add(NewGridData);
		}
	}
}

void UGroundGridMgrComponent::GetTouchGrid(FVector TouchLocation , FGridData& GridData)
{
	FVector RelativeLocation = FVector(TouchLocation.X - GroundActor->GetTopRightLocation().X, TouchLocation.Y - GroundActor->GetTopRightLocation().Y, GroundActor->GetTopRightLocation().Z);
	FVector CalculateLocation = RelativeLocation.GetAbs();

	FVector LeftButtomLocation = GroundActor->GetButtomLeftLocation();

	//因为是Ceil以及下面会减一，所以要对刚好到边上的做特殊处理！
// 	if (CalculateLocation.X == 0)
// 		CalculateLocation.X = 0.1f;
// 	if (CalculateLocation.Y == 0)
// 		CalculateLocation.Y = 0.1f;
// 	if (TouchLocation.X == LeftButtomLocation.X)
// 		CalculateLocation.X += 0.1f;
// 	if (TouchLocation.Y == 0)
// 		CalculateLocation.Y += 0.1f;
	CalculateLocation.X += 0.1f;
	CalculateLocation.Y += 0.1f;

	//判断相对位置落在哪个格子区间 ,
	int32 CurGridRow = UKismetMathLibrary::FCeil(CalculateLocation.X / FGroundUtil::GroundGridWidth);
	int32 CurGridColumn = UKismetMathLibrary::FCeil(CalculateLocation.Y / FGroundUtil::GroundGridHeight);

	if (GridDataListNested.Num() > CurGridRow)
	{
		GridData = GridDataListNested[CurGridRow - 1].Array[CurGridColumn - 1];
	}
}
#pragma optimize("",off)
void UGroundGridMgrComponent::UpdateBlockGrid(TArray<FNestedArray> BlockGridList)
{
	//遍历传来的阻挡格子数据并且更新自身的阻挡格子数据
	for (int32 Row = 0; Row < BlockGridList.Num(); Row++)
	{
		for (int32 Column = 0; Column < GridDataListNested[Column].Array.Num(); Column++)
		{
			FGridData NewGridData = GridDataListNested[Row].Array[Column];
			BlockGridDataList[NewGridData.GridRow].Array[NewGridData.GridColumn] = NewGridData;
			BlockGridDataList[NewGridData.GridRow].Array[NewGridData.GridColumn].BeOccupy = true;
		}
	}
}

bool UGroundGridMgrComponent::SaveCurDecoration()
{
	//如果没有互相遮挡，则可以保存
	if (BeBlockDecorate == false)
	{

	} 

	return BeBlockDecorate;
}

//因为阻挡格子与正式格子同步，所以直接用阻挡格子判断
bool UGroundGridMgrComponent::CheckGridsBeBlock(TArray<FNestedArray> DecorationGridList)
{
	//遍历传来的阻挡格子数据并且更新自身的阻挡格子数据
	for (int32 Row = 0; Row < DecorationGridList.Num(); Row++)
	{
		for (int32 Column = 0; Column < GridDataListNested[Column].Array.Num(); Column++)
		{
			FGridData NewGridData = GridDataListNested[Row].Array[Column];
			if (BlockGridDataList[NewGridData.GridRow].Array[NewGridData.GridColumn].BeOccupy == true )
			{
				return true;
			}

		}
	}
	return false;
}

bool UGroundGridMgrComponent::CheckLocationInGround(FVector &InLocation) const
{
	return GroundActor->CheckLocationInGround(InLocation);
}

bool UGroundGridMgrComponent::CheckLeftBUttomLocationInGround(FVector &LeftButtomLocation)
{
	return GroundActor->CheckLeftBUttomLocationInGround(LeftButtomLocation);
}

#pragma optimize("",on)

