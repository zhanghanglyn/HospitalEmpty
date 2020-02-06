#include "CommonLibrary.h"

FString FCommonLibrary::GetEnumValString(int EnumToGet, FString EnumType)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, EnumType.GetCharArray().GetData(), true);//TEXT("EDecorationType"), true);
	FString CurShooterDataStr(EnumPtr->GetNameByValue((int)EnumToGet).ToString());//转化相应枚举字符串

	FString ClassName;

	CurShooterDataStr.Split("::", nullptr, &ClassName);

	return ClassName;
}