#pragma once

#include "CoreMinimal.h"
#include "Util/StateEnum.h"
#include "TransParam.h"
#include "FSMMgr.generated.h"

/*
	有限状态机，暂时用于玩家操作类
	自身不使用Tick，通过外部调用UpdateState更新
*/
UCLASS(BlueprintType)
class UFSMMgr : public UObject
{
	GENERATED_BODY()

public:
	UFSMMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	/*初始化状态机*/
	void Init( EStateEnum InStateID, class UFSMStateBase* InState);
	/* 添加一个状态 */
	void AddState(EStateEnum InStateID, class UFSMStateBase* InState);
	/* 删除一个状态 */
	void RemoveState(EStateEnum InStateID);
	void RemoveState(class UFSMStateBase* InState);
	/* 切换状态 */
	EStateEnum TransState(ETransConditionID InTransConditionID , UTransParamBase* InParamObj);

	/* 更新状态 */
	void UpdateState();
	/* 响应点击 */
	void OnMouseClickStart();
	void OnMouseClickMove();
	void OnMouseClickEnd();
	void OnMouseHover();

	/*获取当前状态ID*/
	EStateEnum GetCurState() {
		return CurStateID;
	};

	/* 获取当前FSM状态 */
	class UFSMStateBase* GetCurFSMState()
	{
		return CurFSMState;
	};
	
protected:
	/* 设置当前状态，只在初始化使用 */
	void SetCurFSMState(EStateEnum InStateID);

protected:

	/* 当前状态ID */
	UPROPERTY()
	EStateEnum CurStateID = EStateEnum::NONE;

	/* 当前状态 */
	UPROPERTY()
	class UFSMStateBase* CurFSMState;

	/* 当前在状态机内注册的，每个状态ID对应的状态类 */
	UPROPERTY()
	TMap<EStateEnum , UFSMStateBase*> StateMap;
};
