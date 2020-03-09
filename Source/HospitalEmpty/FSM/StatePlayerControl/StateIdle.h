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

	void SetPlayerPawn(class AHptPlayerCameraPawn* InPlayerPawn) {
		PlayerPawn = InPlayerPawn;
	}

	/* 响应点击,如果点击到了家具，则设置家具移动 */
	void OnMouseClickStart();


protected:
	/* 玩家操作器 */
	UPROPERTY()
	class AHptPlayerCameraPawn* PlayerPawn;
};
