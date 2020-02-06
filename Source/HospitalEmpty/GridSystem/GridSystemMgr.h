#pragma once

#include "CoreMinimal.h"
#include "GridSystemMgr.generated.h"

/*
	管理器的基类，包含创建模型等函数
*/
UCLASS()
class UGridSystemMgr :public UObject
{
	GENERATED_BODY()
public:

	static UGridSystemMgr* Get(const UObject* WorldContextObject);

	UGridSystemMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{

	}

	/* 创建一个家具 */
	class ADecorationBase* CreateDecoration() {};
};