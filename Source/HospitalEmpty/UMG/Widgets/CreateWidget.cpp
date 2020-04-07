#include "CreateWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "GameFrame/SaveGameSystem/testObj/TestSerializeObj.h"
#include "GameFrame/SaveGameSystem/testObj/ChildSerializeObj.h"
#include "Base/HptGameInstance.h"
#include "GameFrame/SaveGameSystem/SaveGameSystem.h"
#include "Engine.h"
#include "GameFrame/SaveGameSystem/TestObjNew/BaseSerializeObj.h"

UCreateWidget::UCreateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCreateWidget::NativeConstruct(){
	Super::NativeConstruct();

	//获取一下家具的系统
	DecorationSystemMgr = UDecorationSystemMgr::Get(this);

	/*
	//加载两个button并且添加到List中并且设置点击函数  测试函数，先为不同的ITEM设置不同的回调函数
	for (int32 BtnCount = 0; BtnCount < 14; BtnCount++)
	{
		UButton* item = NewObject<UButton>(this);
		FScriptDelegate Del;

		FString AA = "OnClickedListItem" + FString::FromInt(BtnCount);

		Del.BindUFunction(this, FName(*AA));//"OnClickedListItem");
		item->OnClicked.Add(Del);
		ListView->AddChildToList(item);
	}

	ListView->BindMouseButtonDownCall(this, FName("OnClickListView"),"test param");*/

};

void UCreateWidget::OnClickListView(FString Param, FString TestParam)
{
	
	FString aaa = TestParam;
	int32 aaa1 = 1;
	//UE_LOG(LogTemp, Warning, TEXT("OnClickListView :  TestParam : %s" ), TestParam);
}

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

	//USerializeSystemNew* SerializeSystem = USerializeSystemNew::Get(this);
	//if (SerializeSystem)
	//{
	//	SerializeSystem->LoadActorData(this,"11");
	//}
	if(USaveGameSystem* SaveGameSystem = USaveGameSystem::Get(this))
		SaveGameSystem->LoadGame(this, "Save11");

	//CreateObjAndSerialize();
}

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

/************************************************************************/
/*                      新的以蓝图的方式来处理                          */
/************************************************************************/
#pragma optimize("",off)
TArray<FDecorationInfo> UCreateWidget::UpdateDecInfo()
{
	TArray<FDecorationInfo> ReturnDecInfos;

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDecorationType"), true);
	int32 DecorationTypes = EnumPtr->NumEnums();

	for (int32 count = 0; count < (DecorationTypes - 1); count++)
	{
		FDecorationInfo CurInfo;
		CurInfo.DecorationType = (EDecorationType)(EnumPtr->GetValueByIndex(count));
		CurInfo.DecorationCount = 1;

		ReturnDecInfos.Add(CurInfo);
	}

	return ReturnDecInfos;
}

void UCreateWidget::ClearCurList()
{
	ListView->ClearChildren();
}

#pragma optimize("",on)