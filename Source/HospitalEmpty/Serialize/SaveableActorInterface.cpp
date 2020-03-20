#include "SaveableActorInterface.h"

USaveableActorInterface::USaveableActorInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ISaveableActorInterface::RePointRefurrence(TArray<FRefurrenceData> InRefurrenceData, TMap<FString, UObject *> InSerializeObjList)
{
	int32 a = 1;
	int32 b = 2;
}