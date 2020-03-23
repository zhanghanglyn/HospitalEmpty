#include "StateIdle.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "GameBase/GroundObject/GroundObj.h"
#include "GameBase/DecorationObj/DecorationBase.h"
//#include "GroundObject/GroundGridMgrComponent.h"
#include "FSM/FSMMgr.h"


#pragma optimize("",off)
void UStateIdle::OnMouseClickStart()
{
	if (PlayerPawn == nullptr)
		return;

	//获取点击到的家具
	FHitResult HitResult;
	FVector HitGroundLocation;
	ADecorationBase* HitDecoration = Cast<ADecorationBase>( PlayerPawn->GetMouseHitDecoration(HitResult , HitGroundLocation) );
	if (HitDecoration != nullptr)
	{
		UStateArrangeParam* TempParam = NewObject<UStateArrangeParam>(FSMMgr);
		TempParam->CurDecoration = HitDecoration;
		TempParam->CurGridGround = HitDecoration->GetGround();
		TempParam->LastDecorationLocation = HitGroundLocation; //HitDecoration->GetActorLocation();//HitGroundLocation;

		/* 要在自身地面上，先把自身清除再重新添加 */
		if( AGroundObj* CurGround = HitDecoration->GetGround())
			CurGround->DeleteDecoration(HitDecoration);

		FSMMgr->TransState(ETransConditionID::C_IdleClickToArrange, TempParam);
		TempParam->RemoveFromRoot();
		TempParam = nullptr;
	}
}
#pragma optimize("",on)