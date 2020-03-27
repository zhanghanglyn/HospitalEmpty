#include "SaveDataPlWidget.h"
#include "Base/HptGameInstance.h"
#include "GameFrame/LoadMapSystem/LoadMapSystem.h"
#include "GameFrame/SaveGameSystem/SaveGameSystem.h"
#include "UMG/Widgets/SaveLoadPL/SaveDataItemWidget.h"

USaveDataPlWidget::USaveDataPlWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void USaveDataPlWidget::NativeConstruct() {
	Super::NativeConstruct();

};

void USaveDataPlWidget::AddSaveItemToList()
{
	//根据有多少个存档显示档位ITEM
	if (USaveGameSystem* SaveGameSystem = USaveGameSystem::Get(this))
	{
		FGameSaveData CurrentData = SaveGameSystem->GetGameSaveData();
		for (TMap<FString, FSaveDataListStruct>::TConstIterator Iter(CurrentData.StructData) ; Iter; ++Iter)
		{
			USaveDataItemWidget* item = CreateWidget<USaveDataItemWidget>(this, USaveDataItemWidget::StaticClass());
			
			FSaveDataListStruct ItemData = Iter->Value;

			item->DataName->SetText(FText::FromString(ItemData.NameData));
			item->SaveTime->SetText(FText::FromString(ItemData.SaveTime));
			//item->DataPic->SetBrushFromTexture();
			item->GameID = ItemData.GameID;

			item->MouseClickDelegate.BindUFunction(this, "ClickDataCall");

			SaveList->AddChildToList(item);
		}
		
	}
}

void USaveDataPlWidget::ClickDataCall(FString GameID)
{
	if (USaveGameSystem* SaveGameSystem = USaveGameSystem::Get(this))
	{
		SaveGameSystem->LoadGame(this, GameID);
	}
}