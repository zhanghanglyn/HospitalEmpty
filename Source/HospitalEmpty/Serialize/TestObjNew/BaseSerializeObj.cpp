#include "BaseSerializeObj.h"
#include "SerializeComponent.h"
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
}