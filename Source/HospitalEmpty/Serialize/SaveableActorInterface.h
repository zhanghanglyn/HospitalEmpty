#pragma once

#include "Serialize/SerializeSystemNew.h"
#include "SaveableActorInterface.generated.h"


UINTERFACE(BlueprintType)
class USaveableActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/* 用于Object继承的存储类接口 */
class ISaveableActorInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	/* 
		重定向所有的指针引用 
		Param :  Obj	因为在接口中取不到自身，所以传入
			     InRefurrenceData 自身的引用数据
				 InRefurrenceArrayData 自身的引用数据
				 InSerializeObjList 序列化出来的所有Obj
	*/
	UFUNCTION()
	virtual void RePointRefurrence( UObject* Obj, TArray<FRefurrenceData> InRefurrenceData , TArray<FRefurrenceArrayData> InRefurrenceArrayData,
		TMap<FString, UObject* > InSerializeObjList);

	/*
		由各自的类自行继承调用，在指针的引用重新链接完毕之后，重新刷新一下数据
		Param :  Obj	因为在接口中取不到自身，所以传入
	*/
	UFUNCTION()
	virtual void RefreshAfterRePoint(){};
};