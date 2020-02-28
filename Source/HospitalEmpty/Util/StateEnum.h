#pragma once

#include "CoreMinimal.h"
#include "StateEnum.generated.h"


/*
	如果所有的家具蓝图都放在一起，则可以直接通过枚举的名字来组合路径进行加载！
*/
UENUM()
enum class EDecorationType : uint8
{
	None,
	DecorationBase				UMETA(DisplayName = "测试家具"),		//测试物品
	Sofa						UMETA(DisplayName = "沙发"),
};

/*
	状态ID枚举
*/
UENUM(BlueprintType)
enum class EStateEnum : uint8
{
	NONE					UMETA(DisplayName = "无"),
	IDLE					UMETA(DisplayName = "普通状态"),
	PRE_ARRANGE				UMETA(DisplayName = "预放置家具（无格子）"),
	ARRANGE					UMETA(DisplayName = "布置家具"),
	WALL_ARRANGE			UMETA(DisplayName = "布置墙"),

	CHOOSE_PARIENT			UMETA(DisplayName = "选中病人"),
};

/*
	转换条件的枚举,每一个状态都会包含一个转换条件和对应的状态ID
*/
UENUM(BlueprintType)
enum class ETransConditionID : uint8
{
	NONE					UMETA(DisplayName = "无"),

	/* 玩家控制创建家具相关 */
	C_CREATEFURNITURE		UMETA(DisplayNmae = "从普通状态变成创建家具预布置状态"),
	C_CreateToArrange		UMETA(DisplayNmae = "从预布置变成布置状态"),
	C_CreateToWallArrange	UMETA(DisplayNmae = "从预布置变成布置墙状态"),
	C_PreToIdle				UMETA(DisplayNmae = "从预布置变成普通状态"),
	C_ARRANGEOVER			UMETA(DisplayNmae = "布置家具完成，变为普通状态"),

};

/*
	TransCondition时所使用的参数基类，通过转化为自己需要的类型在自己的State中进行数据的初始化等
*/
UCLASS(BlueprintType)
class UTransParamBase : public UObject
{
	GENERATED_BODY()

public:
	UTransParamBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	};

};