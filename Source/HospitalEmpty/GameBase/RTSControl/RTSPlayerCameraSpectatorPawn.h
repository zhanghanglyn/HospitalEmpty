#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "RTSPlayerCameraSpectatorPawn.generated.h"

/*
RTS视角操作方式APawn,上面会绑上组件作为一个玩家主逻辑的控件
*/
UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "RTS视角操作方式APawn"))
class ARTSPlayerCameraSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	
	ARTSPlayerCameraSpectatorPawn(const FObjectInitializer& ObjectInitializer);

	/*Virtual Function*/
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

public:
	/* 目前暂时用这个简单函数，设置Actor摄像机的位置以及角度等 */
	void UpdateActor();

	/* 向前移动 */
	//UFUNCTION()
	void MoveForward(float diretction);

	/* 向前移动 */
	//UFUNCTION()
	void MoveToRight(float diretction);

	/* 鼠标点击，移动以及抬起事件相关 */
	virtual void OnMouseClickStart();
	virtual void OnMouseClickMove();
	virtual void OnMouseClickEnd();
	/* 鼠标未点击但是移动 */
	virtual void OnMouseHover();
	
	/* 获取当前鼠标在世界中的位置 */
	void GetMouseWorldPos(FVector& WorldLocation, FVector& WorldDirection) const;

protected:
	/* 获取当前点击到的物体 */
	class AActorBase* GetCurTouchObj(FVector &HitLocation);

	//测试函数，点击创建墙
	void TestCreateWall();

public:
	//相机高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraHeight = 1000.0f;

	//相机角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraAngle = -45.0f;

	//移动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MoveSpeed = 30.0f;

protected:
	UCameraComponent* CameraComponent;

protected:
	//当前将要移动到的位置，根据输入实时计算
	FVector CurDestination = FVector(0,0,0);

	//鼠标左键是否被按下
	bool BMouseLeftHold = false;
};