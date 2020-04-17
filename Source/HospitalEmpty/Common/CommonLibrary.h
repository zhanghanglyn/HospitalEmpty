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

	/* 4.17 因为创建一个Volume是通过从左测的factory上创建，需要通过ActorFactory去进行创建，
	如果由代码直接创建可能获取不到当前的ActorFactory，所以将其中的方法提出为一个通用创建Box Volume的方法。 
	*/
#if WITH_EDITOR
	UFUNCTION(  BlueprintCallable , Category = "CommonFunction" , meta = (BlueprintInternalUseOnly = "false" , WorldContext = "WorldContextObject"))
	static AVolume* CreateBoxVolume(class ULevel* InLevel, UClass* InClass, FTransform SpawnTransform, const FName Name = NAME_None);
#endif
};