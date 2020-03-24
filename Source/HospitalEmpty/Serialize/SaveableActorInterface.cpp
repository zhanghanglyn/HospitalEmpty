#include "SaveableActorInterface.h"

USaveableActorInterface::USaveableActorInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
#pragma optimize("",off)
void ISaveableActorInterface::RePointRefurrence(UObject* Obj, TArray<FRefurrenceData> InRefurrenceData , TArray<FRefurrenceArrayData> InRefurrenceArrayData,
	TArray<FRefurrenceMapData> InRefurrenceMapData, TMap<FString, UObject *> InSerializeObjList)
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
	//����һ��TArray������
	for (FRefurrenceArrayData RefArrayData : InRefurrenceArrayData)
	{
		UProperty* OneProperty = ObjClass->FindPropertyByName(RefArrayData.PropertyName);
		if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(OneProperty))
		{
			TArray<UObject*> OuterObject = *ArrayProperty->ContainerPtrToValuePtr<TArray<UObject*>>(Obj);
			TArray<UObject*> ResultObjList;
			void* ValuePtr = ArrayProperty->ContainerPtrToValuePtr<void>(Obj);
			// We need the helper to get to the items of the array            
			FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
			for (int32 i = 0, n = Helper.Num(); i < n; ++i)
			{
				if (UObjectProperty* InnerProperty = Cast<UObjectProperty>(ArrayProperty->Inner))
				{
					UObject* Ojb = InnerProperty->GetPropertyValue(Helper.GetRawPtr(i));
					FString RefData = RefArrayData.SerializeListID[i];
					if (InSerializeObjList.Contains(RefData))
					{
						InnerProperty->SetObjectPropertyValue(Helper.GetRawPtr(i), InSerializeObjList[RefData]);
					}
				}
			}
		}
	}
	//����һ��Map������
	for (FRefurrenceMapData RefMapData : InRefurrenceMapData)
	{
		UProperty* OneProperty = ObjClass->FindPropertyByName(RefMapData.PropertyName);
		if (UMapProperty* MapProperty = Cast<UMapProperty>(OneProperty))
		{
			void* ValuePtr = MapProperty->ContainerPtrToValuePtr<void>(Obj);
			// We need the helper to get to the items of the array            
			FScriptMapHelper Helper(MapProperty, ValuePtr);
			for (int32 i = 0, n = Helper.Num(); i < n; ++i)
			{
				if (UObjectProperty* InnerProperty = Cast<UObjectProperty>(MapProperty->ValueProp))
				{
					UObject* Ojb = InnerProperty->GetPropertyValue(Helper.GetValuePtr(i));
					FString RefData = RefMapData.SerializeListID[i];
					if (InSerializeObjList.Contains(RefData))
					{
						InnerProperty->SetObjectPropertyValue(Helper.GetValuePtr(i), InSerializeObjList[RefData]);
					}
				}
			}
		}
	}
}
#pragma optimize("",on)