#include "StateArrange.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "Actor/GroundDefaultActor.h"
#include "GroundObj.h"
#include "DecorationBase.h"
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