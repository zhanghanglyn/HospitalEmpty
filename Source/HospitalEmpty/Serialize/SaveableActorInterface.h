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
				 InSerializeObjList 序列化出来的所有Obj
	*/
	UFUNCTION()
	virtual void RePointRefurrence( UObject* Obj, TArray<FRefurrenceData> InRefurrenceData ,TMap<FString, UObject* > InSerializeObjList);

};