#pragma once

#include "CoreMinimal.h"
#include "Util/StateEnum.h"
#include "TransParam.generated.h"

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


/*
	拖动状态所需要的使用的参数
*/
UCLASS()
class UStateArrangeParam : public UTransParamBase
{
	GENERATED_BODY()
public:
	UStateArrangeParam(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	};

	/* 家具类型，即使对应的BP名字 */
	UPROPERTY()
		class ADecorationBase* CurDecoration;

	/* 当前操作/保存的地面 */
	UPROPERTY()
		class AGroundObj* CurGridGround;

	UPROPERTY()
		FVector LastDecorationLocation;

};


/*
	预拖动状态所需要的使用的参数
*/
UCLASS()
class UPreArrangeParam : public UTransParamBase
{
	GENERATED_BODY()
public:
	UPreArrangeParam(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	};

	/* 家具类型，即使对应的BP名字 */
	UPROPERTY()
	EDecorationType DecorationType;

};