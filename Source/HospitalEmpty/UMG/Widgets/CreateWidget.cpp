#include "CreateWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "Serialize/testObj/TestSerializeObj.h"
#include "Serialize/testObj/ChildSerializeObj.h"
#include "Base/HptGameInstance.h"
#include "Serialize/SerializeSystemNew.h"
#include "Engine.h"
#include "Serialize/TestObjNew/BaseSerializeObj.h"

UCreateWidget::UCreateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCreateWidget::NativeConstruct(){
	Super::NativeConstruct();

	//获取一下家具的系统
	DecorationSystemMgr = UDecorationSystemMgr::Get(this);

	//加载两个button并且添加到List中并且设置点击函数  测试函数，先为不同的ITEM设置不同的回调函数
	for (int32 BtnCount = 0; BtnCount < 4; BtnCount++)
	{
		UButton* item = NewObject<UButton>(this);
		FScriptDelegate Del;

		FString AA = "OnClickedListItem" + FString::FromInt(BtnCount);

		Del.BindUFunction(this, FName(*AA));//"OnClickedListItem");
		item->OnClicked.Add(Del);
		ListView->AddChildToList(item);
	}
};

void UCreateWidget::AddToListView(UWidget* Content)
{
	if (ListView)
	{
		ListView->AddChildToList(Content);
	}
}

/*
	设置DecorationSystemMgr的状态
*/
void UCreateWidget::OnClickedListItem0()
{
	//test
	//CreateSerializeObj();
	//return;

	UE_LOG(LogTemp, Warning, TEXT("000000"));
	
	if (DecorationSystemMgr)
	{
		AHptPlayerCameraPawn* PlayerPawn = DecorationSystemMgr->GetPlayerCameraPawn();
		if (PlayerPawn)
			PlayerPawn->SetStatePreArrange(EDecorationType::DecorationBase);
	}
		
}
void UCreateWidget::OnClickedListItem1()
{
	//test
	//SaveSerialize();
	//return;

	UE_LOG(LogTemp, Warning, TEXT("11111111111"));
	if (DecorationSystemMgr)
	{
		AHptPlayerCameraPawn* PlayerPawn = DecorationSystemMgr->GetPlayerCameraPawn();
		if (PlayerPawn)
			PlayerPawn->SetStatePreArrange(EDecorationType::Sofa);
	}
}
void UCreateWidget::OnClickedListItem2()
{
	//test
	//LoadSerialize();
	//return;
	ABaseSerializeObj* Actor = Cast<ABaseSerializeObj>(UGameplayStatics::GetActorOfClass(this, ABaseSerializeObj::StaticClass()));
	if (Actor)
	{
		Actor->AddTestSerializeObj();
	}

	UE_LOG(LogTemp, Warning, TEXT("2222222"));
}

void UCreateWidget::OnClickedListItem3()
{

	USerializeSystemNew* SerializeSystem = USerializeSystemNew::Get(this);
	if (SerializeSystem)
	{
		SerializeSystem->LoadActorData(this,"11");
	}

	//CreateObjAndSerialize();
}

#pragma optimize("",off)
void UCreateWidget::CreateSerializeObj()
{
	//ATestSerializeObj
	UWorld* world = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (world)
	{
		TestSerializeObj = world->SpawnActor<ATestSerializeObj>(ATestSerializeObj::StaticClass());
		//if (TestSerializeObj)
		//{
			//TestSerializeObj->ChildObj = NewObject<AChildSerializeObj>(TestSerializeObj);
		//}
	}

}

void UCreateWidget::SaveSerialize()
{
	if (TestSerializeObj)
		TestSerializeObj->SaveObjectToFile("SerializeTest1");
}

void UCreateWidget::LoadSerialize()
{
	if (TestSerializeObj)
		TestSerializeObj->LoadObjectFromFile("SerializeTest1");
}

void UCreateWidget::CreateObjAndSerialize()
{
	UWorld* world = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (world)
	{
		TestSerializeObj = world->SpawnActor<ATestSerializeObj>(ATestSerializeObj::StaticClass());
	}
}

#pragma optimize("",on)