#pragma once

#include "CoreMinimal.h"
#include "RTSMode/Private/RTSControl/RTSPlayerCameraSpectatorPawn.h"
#include "HptPlayerCameraPawn.generated.h"

/*
RTS视角操作方式APawn,上面会绑上组件作为一个玩家主逻辑的控件
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

protected:

public:

};