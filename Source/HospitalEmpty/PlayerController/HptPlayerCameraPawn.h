#pragma once

#include "CoreMinimal.h"
#include "Util/StateEnum.h"
#include "GameBase/RTSControl/RTSPlayerCameraSpectatorPawn.h"
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
	UFUNCTION(BlueprintCallable)
	/* 获取当前鼠标点在地面上的位置 ,并且返回当前的格子地面
	   地面即为GridGround，之后肯定会有多个地面，或是多个Ground某些不需要格子的情况
	*/
	class AActorBase* GetMouseLocationInGround( FVector &GroundLocation) const;

	/*
		切换状态机为预创建家具状态
	*/
	UFUNCTION(BlueprintCallable)
	void SetStatePreArrange(EDecorationType InDecorationType);

	/* 获取当前鼠标点是否有点中家具，如果有返回点中的家具 , InHitResult为射中的家具的射中信息 */
	UFUNCTION(BlueprintCallable)
	class AActorBase* GetMouseHitDecoration(FHitResult &InHitResult , FVector &GroundLocation) const;

protected:
	/* 初始化状态机 */
	void InitFSM();

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

	/* 状态机 */
	UPROPERTY()
	class UFSMMgr* FSMMgr;
};