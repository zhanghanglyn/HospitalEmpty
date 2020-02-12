#include "HptGameInstance.h"
#include "HospitalEmpty/GridSystem/DecorationSystemMgr.h"

void UHptGameInstance::Init()
{
	if (DecorationSystemMgr == nullptr)
		DecorationSystemMgr = NewObject<UDecorationSystemMgr>(this, TEXT("DecorationSystemMgr"));
}

void UHptGameInstance::Shutdown()
{

}
