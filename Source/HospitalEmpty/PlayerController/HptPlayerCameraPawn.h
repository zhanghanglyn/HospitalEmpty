﻿#pragma once

#include "CoreMinimal.h"
#include "RTSMode/Private/RTSControl/RTSPlayerCameraSpectatorPawn.h"
#include "HptPlayerCameraPawn.generated.h"

/*
RTS视角操作方式APawn,上面会绑上组件作为一个玩家主逻辑的控件
玩家控制器只与各个系统的MGR进行交互，并不会因此改变自身状态
*/
UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "RTS视角操作方式APawn"))
class AHptPlayerCameraPawn : public ARTSPlayerCameraSpectatorPawn
{
	GENERATED_BODY()

public:
	
	AHptPlayerCameraPawn(const FObjectInitializer& ObjectInitializer);

	/*Virtual Function*/
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	virtual void BeginPlay() override;

	/* 鼠标点击，移动以及抬起事件相关 */
	virtual void OnMouseClickStart() override;
	virtual void OnMouseClickMove() override;
	virtual void OnMouseClickEnd() override;

	virtual void OnMouseHover() override;

public:
	/* 获取当前鼠标点在地面上的位置 ,并且返回当前的格子地面
	   地面即为GridGround，之后肯定会有多个地面，或是多个Ground某些不需要格子的情况
	*/
	class AGroundObj* GetMouseLocationInGround( FVector &GroundLocation ) const;


protected:
	UPROPERTY( EditAnywhere , BlueprintReadWrite)
	float RayLength = 115000;

protected:
	
	/* 当前格子地面 */
	UPROPERTY()
	class AActorBase* CurGridGround;

	/* 家具系统管理器 */
	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	UPROPERTY()
	class ADecorationBase* TestCurDecoration;

};