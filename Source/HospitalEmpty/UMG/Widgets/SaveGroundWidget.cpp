#include "SaveGroundWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "Serialize/TestSerializeObj.h"
#include "Serialize/ChildSerializeObj.h"
#include "GroundObj.h"
#include "Engine.h"

USaveGroundWidget::USaveGroundWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void USaveGroundWidget::NativeConstruct(){
	Super::NativeConstruct();

	DecorationSystemMgr = UDecorationSystemMgr::Get(this);

	if (SaveButton)
	{
		FScriptDelegate Del;
		Del.BindUFunction(this, "SaveGround");
		SaveButton->OnClicked.Add(Del);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SaveButton->Slot))
			CanvasSlot->SetPosition(FVector2D(50, 50));

	}

};

void USaveGroundWidget::SaveGround()
{
	if (DecorationSystemMgr)
	{
		TArray<AActor*> AllGround = DecorationSystemMgr->GetAllGridGround(this);
		for (int32 GroundNum = 0; GroundNum < AllGround.Num(); GroundNum++)
		{
			if (AGroundObj* GroundObj = Cast<AGroundObj>(AllGround[GroundNum]))
			{
				GroundObj->SaveObjectToFile("TTTTTT1");
			}

		}
	}
		
}