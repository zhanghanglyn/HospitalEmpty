﻿#include "TestObject.h"
#include "Engine/World.h"
#include "UMG/Widgets/CreateWidget.h"
#include "Components/Button.h"
#include "Base/HptGameInstance.h"

ATestObject::ATestObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ATestObject::LoadUI()
{
	UHptGameInstance* Instance = UHptGameInstance::GetInstance(this);
	UUMGManager* UMGManager = Instance->GetUMGManager();

	UWorld* MyWorld = GetWorld();

	CreateWidget = UMGManager->CreateScreenWidget( MyWorld , CreateUIPath);
	UCreateWidget* Widget = Cast<UCreateWidget>(CreateWidget);

	//加载两个button并且添加到List中并且设置点击函数  测试函数，先为不同的ITEM设置不同的回调函数
	for (int32 BtnCount = 0; BtnCount < 3; BtnCount++)
	{
		UButton* item = NewObject<UButton>(Widget);
		FScriptDelegate Del;

		FString AA = "OnClickedListItem" + FString::FromInt(BtnCount);

		Del.BindUFunction(this, FName(*AA));//"OnClickedListItem");
		item->OnClicked.Add(Del);
		Widget->AddToListView(item);
	}

}

void ATestObject::OnClickedListItem0()
{
	UE_LOG(LogTemp , Warning, TEXT("000000"));
}
void ATestObject::OnClickedListItem1()
{
	UE_LOG(LogTemp, Warning, TEXT("11111111111"));
}
void ATestObject::OnClickedListItem2()
{
	UE_LOG(LogTemp, Warning, TEXT("2222222"));
}
