#pragma once

#include "CoreMinimal.h"
#include "OverrideUMGInterface.generated.h"

/*
	Override的UI的绑定自定义参数回调
*/
UCLASS(BlueprintType)
class UUMGParamBase : public UObject
{
	GENERATED_BODY()

public:
	UUMGParamBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer){};

};

/*
	使用一个类似接口的基类，来实现自定义UMG回调
*/
class UMGInterFace
{

	DECLARE_DELEGATE_ThreeParams(DMouseBtnDownCall, const FGeometry&, const FPointerEvent& , UUMGParamBase*)
	DECLARE_DELEGATE_ThreeParams(DMouseBtnDownMoveCall, const FGeometry&, const FPointerEvent&, UUMGParamBase*)
	DECLARE_DELEGATE_ThreeParams(DMouseBtnUpCall, const FGeometry&, const FPointerEvent&, UUMGParamBase*)

public:
	UMGInterFace() {};

	/* 在此可以提供几个方法，根据传入的Geometry以及MouseEvent等获取位置啊之类的 */
	//void XXXX;

public:
	//开始点击的委托
	DMouseBtnDownCall DelegateMouseBtnDownCall;


};
