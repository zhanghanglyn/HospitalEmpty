#include "SaveDataItemWidget.h"
#include "Base/HptGameInstance.h"
#include "GameFrame/LoadMapSystem/LoadMapSystem.h"
#include "GameFrame/SaveGameSystem/SaveGameSystem.h"

USaveDataItemWidget::USaveDataItemWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void USaveDataItemWidget::NativeConstruct() {
	Super::NativeConstruct();

	//DataPic = GetWidgetFromName("Pic");

};

//FEventReply USaveDataItemWidget::OnMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
//{
//	FEventReply Reply = new FEventReply();
//	Reply.NativeReply = FReply::Handled();
//
//	if (MouseClickDelegate.IsBound())
//		MouseClickDelegate.Execute(GameID);
//
//
//	return Reply;
//}