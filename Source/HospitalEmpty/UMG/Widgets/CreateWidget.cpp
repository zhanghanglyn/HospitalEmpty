#include "CreateWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"

UCreateWidget::UCreateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCreateWidget::NativeConstruct(){
	Super::NativeConstruct();

	//获取一下家具的系统
	DecorationSystemMgr = UDecorationSystemMgr::Get(this);

	//加载两个button并且添加到List中并且设置点击函数  测试函数，先为不同的ITEM设置不同的回调函数
	for (int32 BtnCount = 0; BtnCount < 3; BtnCount++)
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
	UE_LOG(LogTemp, Warning, TEXT("2222222"));
}