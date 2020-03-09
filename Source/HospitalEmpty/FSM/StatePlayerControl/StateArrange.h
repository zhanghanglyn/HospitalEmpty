#pragma once

#include "CoreMinimal.h"
#include "FSM/FSMStateBase.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "StateArrange.generated.h"

UCLASS(BlueprintType)
class UStateArrange : public UFSMStateBase
{
	GENERATED_BODY()
public:
	UStateArrange(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
		CurStateID = EStateEnum::ARRANGE;
	};

	virtual void BeforeEnter(UTransParamBase* InParamObj) override ;
	virtual void BeforeExit() override {};
	virtual void Update() override {};
	virtual void BreakCondition() override;
	virtual void AfterExit() override;
	
	/* 响应点击 */
	virtual void OnMouseClickStart();
	virtual void OnMouseClickMove() {};
	virtual void OnMouseClickEnd() {};
	virtual void OnMouseHover();

	void SetPlayerPawn(class AHptPlayerCameraPawn* InPlayerPawn) {
		PlayerPawn = InPlayerPawn;
	}

	void SetDecorationSystemMgr(class UDecorationSystemMgr* InDecorationSystemMgr)
	{
		DecorationSystemMgr = InDecorationSystemMgr;
	}

protected:
	/* 玩家操作器 */
	UPROPERTY()
	class AHptPlayerCameraPawn* PlayerPawn;

	/* 当前操作/保存的地面 */
	UPROPERTY()
	class AGroundObj* CurGridGround;

	/* 当前操作/处理的家具 */
	UPROPERTY()
	class ADecorationBase* CurDecoration;

	/* 家具系统管理器 */
	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	/*
	上一次的移动家具位置,
	用来处理如果鼠标位置移出了地面，Location会为Null导致报错问题
*/
	UPROPERTY()
	FVector LastDecorationLocation;

	/* 当为True时，跳出回到Idle界面 */
	UPROPERTY()
	bool BBackToIdle = false;


};