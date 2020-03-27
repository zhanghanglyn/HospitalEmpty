#include "ChildSerializeObj.h"

void AChildSerializeObj::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << testTextChild;

	UE_LOG(LogTemp , Warning , TEXT("Serialize  CHILD!!! Child!!!!!"));
}

FArchive& operator<<(FArchive& Ar, AChildSerializeObj& SaveActorRef)
{
	Ar << SaveActorRef.testTextChild;

	return Ar;
}