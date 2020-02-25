#include "FSMStateBase.h"

void UFSMStateBase::AddCondition(ETransConditionID InTransCondition, EStateEnum InState)
{
	if (InTransCondition == ETransConditionID::NONE)
		return;

	if (TransTOStateMap.Contains(InTransCondition))
		return;

	TransTOStateMap.Add(InTransCondition, InState);
}

void UFSMStateBase::RemoveCondition(ETransConditionID InTransCondition)
{
	if (TransTOStateMap.Contains(InTransCondition))
		TransTOStateMap.Remove(InTransCondition);
}

EStateEnum UFSMStateBase::GetTransState(ETransConditionID InTransCondition)
{
	//如果不包含，则返回None
	if (!TransTOStateMap.Contains(InTransCondition))
		return EStateEnum::NONE;

	return TransTOStateMap[InTransCondition];

}