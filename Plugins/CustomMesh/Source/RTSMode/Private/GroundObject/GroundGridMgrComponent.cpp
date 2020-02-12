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
			GridLocation.X = StartLocation.X - Column * FGroundUtil::GroundGridWidth;
			GridLocation.Y = StartLocation.Y + Row * FGroundUtil::GroundGridHeight;
			NewGridData.StartLocation = GridLocation;

			//GridDataList.Add(NewGridData);
			if (GridDataListNested.Num() <= Row)
			{
				FNestedArray tempArray;
				GridDataListNested.Add(tempArray);
			}
			GridDataListNested[Row].Array.Add( NewGridData);
		}
	}
}

#pragma optimize("",off)
void UGroundGridMgrComponent::GetTouchGrid(FVector TouchLocation , FGridData& GridData)
{
	FVector RelativeLocation = FVector(TouchLocation.X - GroundActor->GetTopRightLocation().X, TouchLocation.Y - GroundActor->GetTopRightLocation().Y, GroundActor->GetTopRightLocation().Z);
	TouchLocation = RelativeLocation.GetAbs();

	//判断相对位置落在哪个格子区间 ,
	int32 CurGridRow = UKismetMathLibrary::FCeil(TouchLocation.X / FGroundUtil::GroundGridWidth);
	int32 CurGridColumn = UKismetMathLibrary::FCeil(TouchLocation.Y / FGroundUtil::GroundGridHeight);

	if (GridDataListNested.Num() > CurGridRow)
	{
		GridData = GridDataListNested[CurGridRow - 1].Array[CurGridColumn - 1];
	}
}
#pragma optimize("",on)

