#include "HptGameInstance.h"
#include "HospitalEmpty/GridSystem/GridSystemMgr.h"

void UHptGameInstance::Init()
{
	if (GridSystemMgr == nullptr)
		GridSystemMgr = NewObject<UGridSystemMgr>(this, TEXT("GridSystemMgr"));
}

void UHptGameInstance::Shutdown()
{

}
