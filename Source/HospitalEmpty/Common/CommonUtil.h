#pragma once

/*
	该文件，用来进行一些通用的Struct，UObject等结构的定义
*/

#include "CoreMinimal.h"
#include "CommonUtil.generated.h"


/*
定义一个用来存储 回调的Outer FunctionName 以及参数的集合
*/
USTRUCT(BlueprintType)
struct FOuterFuncNameParamStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	UObject* Outer;

	UPROPERTY()
	FString FunctionName;

	UPROPERTY()
	UObject* Param;

};