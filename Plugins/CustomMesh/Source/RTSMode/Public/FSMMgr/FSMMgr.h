#pragma once

#include "CoreMinimal.h"
#include "FSMMgr.generated.h"

/*
	一个运行于家具内的有限状态机
*/

UCLASS(BlueprintType)
class UFSMMgr : public UObject
{
	GENERATED_BODY()

public:
	UFSMMgr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};


};
