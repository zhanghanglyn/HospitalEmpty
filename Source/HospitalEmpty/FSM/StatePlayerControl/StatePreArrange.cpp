#include "FSM/StatePlayerControl/StatePreArrange.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "Actor/GroundDefaultActor.h"
#include "GameBase/GroundObject/GroundObj.h"
#include "GameBase/DecorationObj/DecorationBase.h"
#include "FSM/FSMMgr.h"
#include "Decoration/Wall.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "FSM/StatePlayerControl/StateArrange.h"

/* 要在此预先创建一个家具 */
void UStatePreArrange::BeforeEnter(UTransParamBase* InParamObj)
{
	EDecorationType CreateType = EDecorationType::None;
	if (UPreArrangeParam* ArrangeParam = Cast<UPreArrangeParam>(InParamObj))
	{
		CreateType = ArrangeParam->DecorationType;
	}
	

	FVector CreateLocation;
	AActorBase* ResultGround = PlayerPawn->GetMouseLocationInGround(CreateLocation);
	CurGridGround = Cast<AGroundObj>(ResultGround);
	/* 如果在正式地面上，直接切换状态为正式布置状态 */
	if (CurGridGround != nullptr)
	{
		CreateLocation.Z += 1;
		CurDecoration = DecorationSystemMgr->CreateDecoration(CreateLocation, CurGridGround, CreateType);
		//如果当前家具是墙，则跳转到布置墙的状态
		if (AWall* CurWall = Cast<AWall>(CurDecoration))
		{
			FSMMgr->TransState(ETransConditionID::C_CreateToWallArrange, nullptr);
		}
		else
			FSMMgr->TransState(ETransConditionID::C_CreateToArrange , nullptr);
	}
	//如果在默认地面上
	else
	{
		AGroundDefaultActor* DefaultGround = Cast<AGroundDefaultActor>(ResultGround);
		if (DefaultGround == nullptr)
		{
			//FSMMgr->TransState(ETransConditionID::C_PreToIdle, nullptr);
			BBackToIdle = true;
		}
		else
		{
			CreateLocation.Z += 3;
			CurDecoration = DecorationSystemMgr->CreateDecoration(CreateLocation, DefaultGround, CreateType, false);
		}
		
	}
}

void UStatePreArrange::OnMouseHover()
{
	if (CurDecoration)
	{
		FVector PreLocation = CurDecoration->GetActorLocation();
		FVector MoveToLocation = LastDecorationLocation;
		//如果射到了地面，则直接将家具布置到场景中
		AActorBase* ResultGround = PlayerPawn->GetMouseLocationInGround(MoveToLocation);
		AGroundObj* HitGround = Cast<AGroundObj>(ResultGround);
		if (HitGround != nullptr)
		{
			CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));

			/* 为生成的家具设置地面等相关 */
			CurDecoration->SetGround(HitGround);
			CurGridGround = HitGround;
			CurDecoration->MoveTo(MoveToLocation);
			//CurControlType = ControlType::ArrangeDecoration;
			UStateArrangeParam* TempParam = NewObject<UStateArrangeParam>(FSMMgr);
			TempParam->CurDecoration = CurDecoration;
			TempParam->CurGridGround = CurGridGround;
			TempParam->LastDecorationLocation = LastDecorationLocation;

			if (AWall* CurWall = Cast<AWall>(CurDecoration))
			{
				FSMMgr->TransState(ETransConditionID::C_CreateToWallArrange, TempParam);
			}
			else
				FSMMgr->TransState(ETransConditionID::C_CreateToArrange, TempParam);
			//FSMMgr->TransState(ETransConditionID::C_CreateToArrange , TempParam);
			TempParam->RemoveFromRoot();
			TempParam = nullptr;
		}
		else
		{
			CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
		}
		LastDecorationLocation = MoveToLocation;
	}
}

void UStatePreArrange::BreakCondition()
{
	if (BBackToIdle)
	{
		FSMMgr->TransState(ETransConditionID::C_PreToIdle, nullptr);
		BBackToIdle = false;
	}
		
}

/*要在此清空所有的状态*/
void UStatePreArrange::AfterExit()
{
	CurGridGround = nullptr;
	CurDecoration = nullptr;
}