#include "DecorationGridMgrComponent.h"
#include "GameBase/GroundObject/GroundGridMgrComponent.h"
#pragma optimize("",off)
void UDecorationGridMgrComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	//Ar << GridDataList;
	//Ar << *DecorationActor;
	//Ar << *GroundGridMgr; 
}
#pragma optimize("",on)
FArchive& operator<<(FArchive& Ar, UDecorationGridMgrComponent& SaveRef)
{
	Ar << SaveRef.GridDataList;
	//Ar << *SaveRef.DecorationActor;
	//Ar << *SaveRef.GroundGridMgr;

	return Ar;
}

void UDecorationGridMgrComponent::SetGroundGridMgr(UGroundGridMgrComponent* InGroundGridMgr)
{
	GroundGridMgr = InGroundGridMgr;
}

void UDecorationGridMgrComponent::InitGridMgr(FDecorationConfig ParamData , ADecorationBase* InDecorationActor)
{
	SetDecorationActor(InDecorationActor);

	//初始化格子
	GridRowColumn = FVector2D(ParamData.GridParamData.RowNum, ParamData.GridParamData.ColumnNum);
};

// void UDecorationGridMgrComponent::CreateInitGrid()
// {
// 	//如果并没有总的地面，则报错提示
// 	if (GroundGridMgr == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT(" GridMgr is Not Set Yet "));
// 		return;
// 	}
// 		
// 	/* 根据家具右上角坐标，获取在地面上的格子信息 */
// 	FGridData TempData;
// 	GroundGridMgr->GetTouchGrid( DecorationActor->GetActorLocation(), TempData);
// 
// 	DecorationActor->SetLocationByTopRight(TempData.StartLocation);
// 
// 	/* 更新自身的格子数据 */
// 	UpdateGridInfo(TempData);
// 
// 	/* 更新整个地面的格子信息 */
// 	//GroundGridMgr
// }

/* 目前更新的格子信息并没有用，暂时保存 */
void UDecorationGridMgrComponent::UpdateGridInfo(FGridData TopRightGridData)
{
	GridDataList.Empty();

	FVector TopRightLocation = TopRightGridData.StartLocation;

	for (int32 Row = 0; Row < GridRowColumn.X; ++Row)
	{
		for (int32 Column = 0; Column < GridRowColumn.Y; ++Column)
		{
			FGridData NewGridData;
			NewGridData.SetGridId();

			/* 计算每一个格子的位置并记录 */
			FVector GridLocation = FVector::ZeroVector;
			GridLocation.Z = TopRightLocation.Z;
			GridLocation.X = TopRightLocation.X + Row * FGroundUtil::GroundGridWidth;
			GridLocation.Y = TopRightLocation.Y + Column * FGroundUtil::GroundGridHeight;
			NewGridData.StartLocation = GridLocation;
			NewGridData.GridRow = TopRightGridData.GridRow + Row;
			NewGridData.GridColumn = TopRightGridData.GridColumn + Column;

			//GridDataList.Add(NewGridData);
			if (GridDataList.Num() <= Row)
			{
				FNestedArray tempArray;
				GridDataList.Add(tempArray);
			}
			GridDataList[Row].Array.Add(NewGridData);
		}
	}
}

/*
	根据中间点InLocation取右上角位置，判断属于地面的哪一个格子中，
	根据右上角格子更新家具位置。
*/
void UDecorationGridMgrComponent::MoveTo(FVector InLocation)
{
	FVector TopRightLocation = DecorationActor->GetTopRightLocation(InLocation);

	/* 根据家具右上角坐标，获取在地面上的格子信息,先判断点是否在地面上 */
	GroundGridMgr->CheckLocationInGround(TopRightLocation);

	/* 判断下左下角是否会超出地面，如果会，则设置一下位置 */
	float Width;
	float Height;
	DecorationActor->GetGridWidthHeight(Width, Height);
	FVector LeftButtomLocation = FVector(TopRightLocation.X + Width, TopRightLocation.Y + Height, TopRightLocation.Z);
	if (!GroundGridMgr->CheckLeftBUttomLocationInGround(LeftButtomLocation))
	{
		TopRightLocation = FVector(LeftButtomLocation.X - Width, LeftButtomLocation.Y - Height, TopRightLocation.Z);
	}

	FGridData TempData;
	GroundGridMgr->GetTouchGrid(TopRightLocation, TempData);

	DecorationActor->SetLocationByTopRight(TempData.StartLocation);

	/* 更新自身的格子数据 */
	UpdateGridInfo(TempData);

	/* 更新整个地面的格子信息 */
	GroundGridMgr->UpdateBlockGrid(DecorationActor);
}