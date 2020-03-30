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

FReply USaveDataItemWidget::NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
	FReply Reply = FReply::Handled();

	if (MouseClickDelegate.IsBound())
		MouseClickDelegate.Execute(GameID);


	return Reply;
}