#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonLibrary.generated.h"

UCLASS()
class UCommonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/* 获取Enum对应的值得String */
	UFUNCTION(BlueprintCallable, Category = "CommonFunction", meta = (BlueprintInternalUseOnly = "false"))
	static FString GetEnumValString(int EnumToGet, FString EnumType);

	/* 获取最外层的Level */
	UFUNCTION(BlueprintCallable, Category = "CommonFunction", meta = (BlueprintInternalUseOnly = "false"))
	static UObject* GetOuterLevel( UObject* InObj );

	/* 获取Level的StreamLevelName */
	//UFUNCTION(BlueprintCallable, Category = "CommonFunction", meta = (BlueprintInternalUseOnly = "false"))
	//static UObject* GetStreamLevelName(UObject* InLevel);
};