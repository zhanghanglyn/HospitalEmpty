#include "CommonLibrary.h"

UCommonLibrary::UCommonLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UCommonLibrary::GetEnumValString(int EnumToGet, FString EnumType)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, EnumType.GetCharArray().GetData(), true);//TEXT("EDecorationType"), true);
	FString CurShooterDataStr(EnumPtr->GetNameByValue((int)EnumToGet).ToString());//转化相应枚举字符串

	FString ClassName;

	CurShooterDataStr.Split("::", nullptr, &ClassName);

	return ClassName;
}

UObject* UCommonLibrary::GetOuterLevel(UObject* InObj)
{
	UObject* Top = (UObject*)InObj;
	for (;;)
	{
		UObject* CurrentOuter = Top->GetOuter();

		if (ULevel* OuterLevel = Cast<ULevel>(CurrentOuter))
		{
			return CurrentOuter;
		}

		/* 如果都没有找到Level */  
		if (!CurrentOuter)
		{
			return nullptr;
		}
		Top = CurrentOuter;
	}
}

//UObject* UCommonLibrary::GetStreamLevelName(UObject* InLevel)
//{
//	FString RealLevelName = InLevel->GetFullGroupName(true);
//}