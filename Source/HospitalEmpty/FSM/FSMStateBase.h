#pragma once

#include "CoreMinimal.h"
#include "Util/StateEnum.h"
#include "TransParam.h"
#include "FSMStateBase.generated.h"

UCLASS(BlueprintType)
class UFSMStateBase : public UObject
{
	GENERATED_BODY()
public:
	UFSMStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	/* 添加一个对应的跳转关系，什么条件能够跳转到什么状态 */
	virtual void AddCondition(ETransConditionID InTransCondition , EStateEnum InState);
	/* 移除一个条件 */
	virtual void RemoveCondition(ETransConditionID InTransCondition);
	/* 根据传入的条件ID，获取跳转后的状态ID */
	virtual EStateEnum GetTransState(ETransConditionID InTransCondition);
	
	virtual void SetFSMMgr(class UFSMMgr* InFSMMgr)
	{
		FSMMgr = InFSMMgr;
	};

	virtual void BeforeEnter( UTransParamBase* InParamObj ) {};
	virtual void BeforeExit() {};
	virtual void Update() {};
	virtual void BreakCondition() {};
	virtual void AfterExit() {};
	
	/* 响应点击 */
	virtual void OnMouseClickStart() {};
	virtual void OnMouseClickMove() {};
	virtual void OnMouseClickEnd() {};
	virtual void OnMouseHover() {};

protected:
	UPROPERTY()
	EStateEnum CurStateID = EStateEnum::NONE;

	/* 状态机控制器保存 */
	UPROPERTY()
	class UFSMMgr* FSMMgr;

	/* 转换条件与条件ID的配对MAP */
	UPROPERTY()
	TMap<ETransConditionID, EStateEnum> TransTOStateMap;
};
