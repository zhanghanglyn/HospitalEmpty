#include "StateArrangeWall.h"
#include "Decoration/Wall.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "Actor/GroundDefaultActor.h"
#include "GameBase/GroundObject/GroundObj.h"
#include "GameBase/DecorationObj/DecorationBase.h"
#include "Decoration/Wall.h"
#include "FSM/FSMMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "FSM/StatePlayerControl/StateArrange.h"


/* 把当前在操作的家具保存下来 */
void UStateArrangeWall::BeforeEnter(UTransParamBase* InParamObj)
{
	EDecorationType CreateType = EDecorationType::None;
	if (UStateArrangeParam* ArrangeParam = Cast<UStateArrangeParam>(InParamObj))
	{
		CurDecoration = Cast<AWall>(ArrangeParam->CurDecoration);
		CurGridGround = ArrangeParam->CurGridGround;
		LastDecorationLocation = ArrangeParam->LastDecorationLocation;
	}
}

void UStateArrangeWall::OnMouseClickStart()
{
	//如果是普通状态，点击开始延长墙
	if (CurArrangeState == EWallArrangeState::None_Click)
	{


		CurArrangeState = EWallArrangeState::Click_Extend;
	}
	//如果是延长墙的状态，点击保存
	else if (CurArrangeState == EWallArrangeState::Click_Extend)
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
}

/*当点击拖动时，拉长墙体*/
void UStateArrangeWall::OnMouseClickMove()
{
	if (CurDecoration)
	{
		//PlayerPawn->GetMouseLocationInGround(MoveToLocation);
	}
}

void UStateArrangeWall::OnMouseHover()
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

void UStateArrangeWall::BreakCondition()
{
	if (BBackToIdle)
	{
		FSMMgr->TransState(ETransConditionID::C_PreToIdle, nullptr);
		BBackToIdle = false;
	}
}

void UStateArrangeWall::AfterExit()
{
	CurDecoration = nullptr;
	CurGridGround = nullptr;
}