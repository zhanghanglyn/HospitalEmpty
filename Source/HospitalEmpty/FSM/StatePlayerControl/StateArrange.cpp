#include "StateArrange.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "Actor/GroundDefaultActor.h"
#include "GameBase/GroundObject/GroundObj.h"
#include "GameBase/DecorationObj/DecorationBase.h"
#include "Decoration/Wall.h"
#include "FSM/FSMMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"

/* 把当前在操作的家具保存下来 */
void UStateArrange::BeforeEnter(UTransParamBase* InParamObj)
{
	EDecorationType CreateType = EDecorationType::None;
	if (UStateArrangeParam* ArrangeParam = Cast<UStateArrangeParam>(InParamObj))
	{
		CurDecoration = ArrangeParam->CurDecoration;
		CurGridGround = ArrangeParam->CurGridGround;
		LastDecorationLocation = ArrangeParam->LastDecorationLocation;
	}
}

void UStateArrange::OnMouseClickStart()
{
	if (CurGridGround != nullptr && !CurGridGround->SaveCurDecoration(CurDecoration))
	{
		//保存之后转回Idle状态
		BBackToIdle = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Block is Grid Block！！！！！！！！"));
	}
}

void UStateArrange::OnMouseHover()
{
	if (CurDecoration)
	{
		FVector PreLocation = CurDecoration->GetActorLocation();
		FVector MoveToLocation = LastDecorationLocation;
		PlayerPawn->GetMouseLocationInGround(MoveToLocation);
		//CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
		CurDecoration->MoveTo(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
		LastDecorationLocation = MoveToLocation;
	}
}

/* 点击鼠标后的拖动事件，在此处理墙的创建 */
// void UStateArrange::OnMouseClickMove()
// {
// 	if (CurDecoration)
// 	{
// 		if (AWall* CreateWall = Cast<AWall>(CurDecoration))
// 		{
// 			/*  要根据移动的第一个格子，确定该墙体的增长方向，只有当方向一致时，才在对应方向拉长墙，反向减少墙，
// 				如果移动的方向不一致，则重新创建一段新的墙体。
// 			*/
// 
// 			/* 1、判断当前鼠标是否移动到了下一个格子，如果移动到了下一个格子，根据当前格子的位置，拉长墙体模型 */
// 
// 			/* 2、 */
// 		}
// 	}
// }

void UStateArrange::BreakCondition()
{
	if (BBackToIdle)
	{
		FSMMgr->TransState(ETransConditionID::C_PreToIdle, nullptr);
		BBackToIdle = false;
	}
}

void UStateArrange::AfterExit()
{
	CurDecoration = nullptr;
	CurGridGround = nullptr;
}