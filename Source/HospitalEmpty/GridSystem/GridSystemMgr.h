#pragma once

#include "CoreMinimal.h"
#include "GridSystemMgr.generated.h"

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
	管理器的基类，包含创建模型等函数
*/
UCLASS()
class UGridSystemMgr :public UObject
{
	GENERATED_BODY()
public:

	static UGridSystemMgr* Get(const UObject* WorldContextObject);

	UGridSystemMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	/* 创建一个家具
		Param : CreateLocation  创建位置
				GridGround      创建家具的地面,用来同步家具信息

	*/
	class ADecorationBase* CreateDecoration( FVector Location ,class AGroundObj* GridGround , EDecorationType DecorationType);

	void SetDecorationBPPath(FString InDecorationBPPath) {
		DecorationBPPath = InDecorationBPPath;
	};
protected:


public:


protected:
	//所有家具的蓝图位置，所有蓝图最终显示效果应该为 Blueprint'/Game/XXX/NAME/BP_NAME.BP_NAME_C' 类似
	UPROPERTY()
	FString DecorationBPPath = "Blueprint'/Game/HospitalProject/Blueprints/Decoration/";
};