#include "SaveableActorInterface.h"

USaveableActorInterface::USaveableActorInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
#pragma optimize("",off)
void ISaveableActorInterface::RePointRefurrence(UObject* Obj ,TArray<FRefurrenceData> InRefurrenceData, TMap<FString, UObject *> InSerializeObjList)
{
	UClass* ObjClass = Obj->GetClass();

	for (FRefurrenceData RefurrenceData : InRefurrenceData)
	{
		UProperty* OneProperty = ObjClass->FindPropertyByName(RefurrenceData.PropertyName);
		if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(OneProperty))
		{
			UObject* subObject = ObjProperty->GetObjectPropertyValue_InContainer(Obj);
			//�ٸ���IDȥ�Ҷ�Ӧ�Ķ���ָ�룬���輴�ɡ�
			if (InSerializeObjList.Contains(RefurrenceData.SerializeDataID))
			{
				//subObject = InSerializeObjList[RefurrenceData.SerializeDataID];
				ObjProperty->SetObjectPropertyValue_InContainer(Obj, InSerializeObjList[RefurrenceData.SerializeDataID]);
			}
		}
	}
}
#pragma optimize("",on)