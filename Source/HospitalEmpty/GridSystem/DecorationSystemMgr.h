#pragma once

#include "CoreMinimal.h"
#include "DecorationSystemMgr.generated.h"

/* 
	如果所有的家具蓝图都放在一起，则可以直接通过枚举的名字来组合路径进行加载！
*/
UENUM()
enum class EDecorationType : int8
{
	None,
	DecorationBase,		//测试物品
};


/*
	状态枚举，表明当前家具控制器的状态
	20.2.11 暂时没有使用状态机，后续添加
*/
UENUM()
enum class ControlType : int8
{
	None,
	ArrangeDecoration,		//放置物品中

};


/*
	管理器的基类，包含创建模型等函数
	20.2.11 这应该做成一个所有家具以及地面的综合系统管理器
	当鼠标点击、移动等状态发生时，应该讲事件抛给该MGR，该MGR根据自身的状态，去创建、设置、移动Decoration等。
	这样的好处是，控制器会被某些固定的状态限制，而是通过不同管理器的状态去执行操作。
*/
UCLASS()
class UDecorationSystemMgr :public UObject
{
	GENERATED_BODY()
public:

	static UDecorationSystemMgr* Get(const UObject* WorldContextObject);

	UDecorationSystemMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

/*
	对外部使用的操作方法
*/
public:

	void OnMouseClickStart();
	void OnMouseClickMove() {};
	void OnMouseClickEnd() {};
	void OnMouseHover();

	/* 创建一个家具
		Param : CreateLocation  创建位置
				GridGround      创建家具的地面,用来同步家具信息
	*/
	class ADecorationBase* CreateDecoration( FVector Location ,class AGroundObj* GridGround , EDecorationType DecorationType);

	void SetDecorationBPPath(FString InDecorationBPPath) {
		DecorationBPPath = InDecorationBPPath;
	};

	void SetPlayerPawn(class AHptPlayerCameraPawn* InPlayerPawn);

protected:
	//所有家具的蓝图位置，所有蓝图最终显示效果应该为 Blueprint'/Game/XXX/NAME/BP_NAME.BP_NAME_C' 类似
	UPROPERTY()
	FString DecorationBPPath = "Blueprint'/Game/HospitalProject/Blueprints/Decoration/";

/*  逻辑操作状态相关  */
protected:

	UPROPERTY()
	ControlType CurControlType = ControlType::None;

	/* 保存一个控制器 */
	UPROPERTY()
	class AHptPlayerCameraPawn* PlayerPawn;

	/* 当前操作/保存的地面 */
	UPROPERTY()
	class AGroundObj* CurGridGround;

	/* 当前操作/处理的家具 */
	UPROPERTY()
	class ADecorationBase* CurDecoration;

	/* 
		上一次的移动家具位置,
		用来处理如果鼠标位置移出了地面，Location会为Null导致报错问题
	*/
	UPROPERTY()
	FVector LastDecorationLocation;

};