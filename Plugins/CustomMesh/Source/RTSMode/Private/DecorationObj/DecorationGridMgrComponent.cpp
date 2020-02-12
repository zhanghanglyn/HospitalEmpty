#include "DecorationGridMgrComponent.h"
#include "GroundGridMgrComponent.h"
#include "DecorationBase.h"

void UDecorationGridMgrComponent::SetGroundGridMgr(UGroundGridMgrComponent* InGroundGridMgr)
{
	GroundGridMgr = InGroundGridMgr;
}

void UDecorationGridMgrComponent::UpdateGrid()
{
	//如果并没有总的地面，则报错提示
	if (GroundGridMgr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" GridMgr is Not Set Yet "));
		return;
	}
		
	/* 根据家具的左上角坐标，获取在地面上的格子信息 */
	FGridData TempData;
	GroundGridMgr->GetTouchGrid( DecorationActor->GetActorLocation(), TempData);

	DecorationActor->SetLocationByTopRight(TempData.StartLocation);
	/*  */

}