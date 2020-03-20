﻿#include "TestSerializeObj.h"
#include "ChildSerializeObj.h"
#include "Serialization/BufferArchive.h"
#include "Misc/FileHelper.h"
#include "Serializable.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryReader.h"

void ATestSerializeObj::Serialize(FArchive& Ar)
{
	if(Ar.IsSaving())
		UE_LOG(LogTemp, Warning, TEXT("ATestSerializeObj::Serialize   !! Saving!"));
	if(Ar.IsLoading())
		UE_LOG(LogTemp, Warning, TEXT("ATestSerializeObj::Serialize   !! Loading!!!"));

	Super::Serialize(Ar);

	//Ar << testText;
	//if (ChildObj != nullptr) 
		//Ar << *ChildObj;

}
#pragma optimize("",off)
void ATestSerializeObj::SaveLoadData(FArchive& Ar)
{
	if (Ar.IsSaving())
		UE_LOG(LogTemp, Warning, TEXT("ATestSerializeObj::Serialize   !! Saving!"));
	if (Ar.IsLoading())
		UE_LOG(LogTemp, Warning, TEXT("ATestSerializeObj::Serialize   !! Loading!!!"));

	Ar << testText;
	if (Ar.IsSaving())
	{
		if( ChildObj != nullptr)
			Ar << *ChildObj;
	}
	else if (Ar.IsLoading())
		Ar << *ChildObj;

}

bool ATestSerializeObj::SaveObjectToFile(FString FilePath)
{
	FBufferArchive ToBinary;
	SaveLoadData(ToBinary);

	if (ToBinary.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveObjectToFile wrong , ToBinary.Num is Zero!"));
		return false;
	}

	//将二进制文件保存到本地文件
	FFileHelper::SaveArrayToFile(ToBinary, *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *FilePath));//FilePath);

	ToBinary.FlushCache();
	ToBinary.Empty();

	return true;
}

bool ATestSerializeObj::LoadObjectFromFile(FString FilePath)
{
	TArray<uint8> BinaryArray;

	if (!FFileHelper::LoadFileToArray(BinaryArray, *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *FilePath)))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadObjectFromFile wrong , LoadFileToArray is Null!"));
		return false;
	}
	if (BinaryArray.Num() <= 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadObjectFromFile wrong , BinaryArray.Num is Zero!"));
		return false;
	}

	FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
	FromBinary.Seek(0);

	SaveLoadData(FromBinary);

	FromBinary.FlushCache();
	BinaryArray.Empty();
	FromBinary.Close();

	return true;

}
#pragma optimize("",on)