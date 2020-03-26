#include "BaseSerializeObj.h"
#include "SerializeComponent.h"
#include "Serialize/testObj/ChildSerializeObj.h"
#include "Serialize/testObj/TestSerializeObj.h"

ABaseSerializeObj::ABaseSerializeObj(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GroundDefaultMesh"));
	//SetRootComponent(MeshComponent);

}

void ABaseSerializeObj::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void ABaseSerializeObj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (TestComponent == nullptr)
	{
		TestComponent = NewObject< USerializeComponent>(this);
		TestComponent->aaaaa = "LFDJAIFD";
		TestComponent->RegisterComponent();

		FTstruct Sttt;
		Sttt.aaa = "STTT.AAA";
		Sttt.tttt1.Add("啦啦啦啦啦");
		TestComponent->StructTwo = Sttt;
	}
}

void ABaseSerializeObj::AddTestSerializeObj()
{
	if (TestOOOOO == nullptr)
		TestOOOOO = NewObject<ATestSerializeObj>(this);

	TestOOOOO->testText = " I COME FROM ABaseSerializeObj";
	for (int32 count = 0 ; count < 1 ; count ++)
	{
		AChildSerializeObj* Obj = NewObject< AChildSerializeObj>(this);
		Obj->testTextChild = FString::FromInt(count);
		TestOOOOO->ChildObj.Add(Obj);
	}
	
	//3.24 
	for (int32 count1 = 0; count1 < 1; count1++)
	{
		AChildSerializeObj* Obj = NewObject< AChildSerializeObj>(this);
		Obj->testTextChild = FString::FromInt(count1);
		MapTest.Add(count1,Obj);
	}
}
//
//void ABaseSerializeObj::RePointRefurrence(TArray< FRefurrenceData> InRefurrenceData, TMap<FString, UObject *> InSerializeObjList)
//{
//	UClass* ObjClass = GetClass();
//
//	for (FRefurrenceData RefurrenceData : InRefurrenceData)
//	{
//		UProperty* OneProperty = ObjClass->FindPropertyByName(RefurrenceData.PropertyName);
//		if (UObjectProperty* ObjProperty = Cast<UObjectProperty>(OneProperty))
//		{
//			UObject* subObject = ObjProperty->GetObjectPropertyValue_InContainer(this);
//			//再根据ID去找对应的对象指针，赋予即可。
//			if (InSerializeObjList.Contains(RefurrenceData.SerializeDataID))
//			{
//				//subObject = InSerializeObjList[RefurrenceData.SerializeDataID];
//				ObjProperty->SetObjectPropertyValue_InContainer(this, InSerializeObjList[RefurrenceData.SerializeDataID]);
//			}
//		}
//	}
//}