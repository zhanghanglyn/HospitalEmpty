#include "SaveableActorInterface.h"

USaveableActorInterface::USaveableActorInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
#pragma optimize("",off)
void ISaveableActorInterface::RePointRefurrence(UObject* Obj, TArray<FRefurrenceData> InRefurrenceData ,
	TArray<FRefurrenceArrayData> InRefurrenceArrayData, TMap<FString, UObject *> InSerializeObjList)
{
	UClass* ObjClass = Obj->GetClass();

	for (FRefurrenceData RefurrenceData : InRefurrenceData)
	{
		UProperty* OneProperty = ObjClass->FindPropertyByName(RefurrenceData.PropertyName);
		if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(OneProperty))
		{
			UObject* subObject = ObjProperty->GetObjectPropertyValue_InContainer(Obj);
			//再根据ID去找对应的对象指针，赋予即可。
			if (InSerializeObjList.Contains(RefurrenceData.SerializeDataID))
			{
				//subObject = InSerializeObjList[RefurrenceData.SerializeDataID];
				ObjProperty->SetObjectPropertyValue_InContainer(Obj, InSerializeObjList[RefurrenceData.SerializeDataID]);
			}
		}
	}
	//遍历一下TArray的引用
	for (FRefurrenceArrayData RefArrayData : InRefurrenceArrayData)
	{
		UProperty* OneProperty = ObjClass->FindPropertyByName(RefArrayData.PropertyName);
		if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(OneProperty))
		{
			TArray<UObject*> OuterObject = *ArrayProperty->ContainerPtrToValuePtr<TArray<UObject*>>(Obj);
			TArray<UObject*> ResultObjList;
			//for (int32 count = 0 ; count < OuterObject.Num() ; count++)
			//{
			//	OuterObject[count] = InSerializeObjList.Find()[count];
			//}
			//int32 count = 0;
			//for (FString RefData : RefArrayData.SerializeListID)
			//{
			//	if (InSerializeObjList.Contains(RefData))
			//	{
			//		OuterObject[count] = InSerializeObjList[RefData];
			//		count++;
			//	}
			//	ResultObjList.Add(InSerializeObjList[RefData]);
			//}
			////ArrayProperty->SetPropertyValue_InContainer(Obj, ResultObjList);
			//FScriptArrayHelper Helper(ArrayProperty, NULL);
			//ArrayProperty->SetPropertyValue_InContainer(Obj, ResultObjList.);
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
}
#pragma optimize("",on)