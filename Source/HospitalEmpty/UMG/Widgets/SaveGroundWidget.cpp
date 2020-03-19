#include "SaveGroundWidget.h"
#include "GridSystem/DecorationSystemMgr.h"
#include "PlayerController/HptPlayerCameraPawn.h"
#include "Serialize/testObj/TestSerializeObj.h"
#include "Serialize/testObj/ChildSerializeObj.h"
#include "GroundObj.h"
#include "Engine.h"
#include "Base/HptGameInstance.h"
#include "Serialize/SerializeSystem.h"

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
	//获取一下序列化System
	USerializeSystem* SerializeSystem = USerializeSystem::Get(this);

	if (DecorationSystemMgr)
	{
		TArray<AActor*> AllGround = DecorationSystemMgr->GetAllGridGround(this);
		for (int32 GroundNum = 0; GroundNum < AllGround.Num(); GroundNum++)
		{
			if (AGroundObj* GroundObj = Cast<AGroundObj>(AllGround[GroundNum]))
			{
				//GroundObj->SaveObjectToFile("TTTTTT1");
				SerializeSystem->SaveAllActorData(this);
			}

		}
	}
		
}