#include "CreateWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "Serialize/TestSerializeObj.h"
#include "Serialize/ChildSerializeObj.h"
#include "Engine.h"

USaveGroundWidget::USaveGroundWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void USaveGroundWidget::NativeConstruct(){
	Super::NativeConstruct();

	UButton* item = NewObject<UButton>(this);
	FScriptDelegate Del;
	Del.BindUFunction(this, "SaveGround");
	item->OnClicked.Add(Del);

};

void USaveGroundWidget::SaveGround()
{

}