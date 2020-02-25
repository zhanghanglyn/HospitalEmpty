#pragma once

#include "CoreMinimal.h"
#include "FSM/FSMStateBase.h"
#include "StateIdle.generated.h"

/*
	普通默认的输入状态，暂时没有任何逻辑功能
*/
UCLASS(BlueprintType)
class UStateIdle : public UFSMStateBase
{
	GENERATED_BODY()
public:
	UStateIdle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
		CurStateID = EStateEnum::IDLE;
	};

protected:

};
