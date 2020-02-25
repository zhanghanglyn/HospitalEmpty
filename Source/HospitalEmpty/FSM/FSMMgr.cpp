#include "FSMMgr.h"
#include "FSMStateBase.h"

void UFSMMgr::Init(EStateEnum InStateID, UFSMStateBase* InState)
{
	StateMap.Empty();
	AddState(InStateID, InState);
	SetCurFSMState(InStateID);
}
#pragma optimize("",off)
void UFSMMgr::AddState(EStateEnum InStateID, UFSMStateBase* InState)
{
	/* 如果是空状态 不添加 */
	if (InStateID == EStateEnum::NONE)
		return;

	if (StateMap.Contains(InStateID) == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("this state is aleady have : %d"), (uint8)InStateID);
	}
	else
	{
		StateMap.Add(InStateID, InState);
	}	
}
#pragma optimize("",on)
void UFSMMgr::RemoveState(EStateEnum InStateID)
{
	if (StateMap.Num() < 1)
		return;

	if (StateMap.Contains(InStateID))
	{
		StateMap.Remove(InStateID);
	}

}

void UFSMMgr::RemoveState(class UFSMStateBase* InState)
{
	if (StateMap.Num() < 1)
		return;
	for (TMap<EStateEnum, UFSMStateBase*>::TConstIterator iter = StateMap.CreateConstIterator() ; iter; ++iter)
	{
		if (iter->Value == InState)
		{
			StateMap.Remove(iter->Key);
			return;
		}
	}
}
#pragma optimize("",off)
EStateEnum UFSMMgr::TransState(ETransConditionID InTransConditionID, UTransParamBase* InParamObj)
{
	if (CurFSMState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("当前状态机没有任何状态！"));
		return EStateEnum::NONE;
	}

	EStateEnum tempStateID = CurFSMState->GetTransState(InTransConditionID);
	if (tempStateID == EStateEnum::NONE)  //该条件并不能使该状态转换，返回无状态
		return EStateEnum::NONE;

	if (!StateMap.Contains(tempStateID))
		return EStateEnum::NONE;

	UFSMStateBase* NextFSMState = StateMap[tempStateID];
	if (NextFSMState == nullptr)
		return EStateEnum::NONE;

	//当前状态执行退出回调
	CurFSMState->BeforeExit();
	//下一个状态执行进入回调
	NextFSMState->BeforeEnter(InParamObj);

	//然后清除数据
	CurFSMState->AfterExit();

	CurFSMState = NextFSMState;


	return tempStateID;
}

void UFSMMgr::SetCurFSMState(EStateEnum InStateID)
{
	if (StateMap.Contains(InStateID))
	{
		UFSMStateBase* FSMState = StateMap[InStateID];
		if (FSMState == nullptr)
		{
			UE_LOG( LogTemp , Warning ,TEXT("FSMState is Empty!") );
			return;
		}
		CurFSMState = FSMState;
	}
}
#pragma optimize("",on)
void UFSMMgr::UpdateState()
{
	if (CurFSMState != nullptr)
	{
		CurFSMState->Update();
		CurFSMState->BreakCondition();
	}
}

void UFSMMgr::OnMouseClickStart() 
{
	if (CurFSMState != nullptr)
		CurFSMState->OnMouseClickStart();
}
void UFSMMgr::OnMouseClickMove()
{
	if (CurFSMState != nullptr)
		CurFSMState->OnMouseClickMove();
}
void UFSMMgr::OnMouseClickEnd() 
{
	if (CurFSMState != nullptr)
		CurFSMState->OnMouseClickEnd();
}
void UFSMMgr::OnMouseHover()
{
	if (CurFSMState != nullptr)
		CurFSMState->OnMouseHover();
}