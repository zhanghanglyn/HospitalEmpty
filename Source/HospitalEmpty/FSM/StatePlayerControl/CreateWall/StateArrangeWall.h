#pragma once

#include "CoreMinimal.h"
#include "FSM/FSMStateBase.h"
#include "StateArrangeWall.generated.h"

/*
	当前墙的布置状态
*/
UENUM()
enum class EWallArrangeState : uint8
{
	None_Click			UMETA(DisplayName = "普通移动墙"),
	Click_Extend		UMETA(DisplayName = "开始延长墙"),
	//Click_Save			UMETA(DisplayName = "点击保存墙"),

};

/*
	普通默认的输入状态，暂时没有任何逻辑功能
*/
UCLASS(BlueprintType)
class UStateArrangeWall : public UFSMStateBase
{
	GENERATED_BODY()
public:
	UStateArrangeWall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
		CurStateID = EStateEnum::WALL_ARRANGE;
	};

	virtual void BeforeEnter(UTransParamBase* InParamObj) override;
	virtual void BeforeExit() override {};
	virtual void Update() override {};
	virtual void BreakCondition() override;
	virtual void AfterExit() override;

	/* 响应点击 */
	virtual void OnMouseClickStart();
	virtual void OnMouseClickMove();
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
	/*当前的点击状态*/
	UPROPERTY()
	EWallArrangeState CurArrangeState;

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
