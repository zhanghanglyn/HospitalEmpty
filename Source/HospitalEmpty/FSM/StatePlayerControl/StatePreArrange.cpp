#include "FSM/StatePlayerControl/StatePreArrange.h"
#include "Engine/Engine.h"
#include "HospitalEmpty/Base/HptGameInstance.h"
#include "Actor/GroundDefaultActor.h"
#include "GroundObj.h"
#include "DecorationBase.h"
#include "FSM/FSMMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"

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
		FSMMgr->TransState(ETransConditionID::C_CreateToArrange , nullptr);
	}
	//如果在默认地面上
	else
	{
		AGroundDefaultActor* DefaultGround = Cast<AGroundDefaultActor>(ResultGround);
		if (DefaultGround == nullptr)
		{
			FSMMgr->TransState(ETransConditionID::C_PreToIdle, nullptr);
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
			FSMMgr->TransState(ETransConditionID::C_CreateToArrange , nullptr);
		}
		else
		{
			CurDecoration->SetActorLocation(FVector(MoveToLocation.X, MoveToLocation.Y, PreLocation.Z));
		}
		LastDecorationLocation = MoveToLocation;
	}
}