#pragma once

#include "CoreMinimal.h"
#include "LoadMapSystem.generated.h"

UCLASS(BlueprintType)
class ULoadMapSystem : public UObject
{
	GENERATED_BODY()

	//声明一个加载完成时的回调委托
	//DECLARE_DYNAMIC_DELEGATE(FOnPackageLoaded);
	DECLARE_DELEGATE_ThreeParams(FOnPackageLoaded , const FName& , UPackage* , int32)
	FOnPackageLoaded OnPackageLoaded;

public:
	ULoadMapSystem(const FObjectInitializer& ObjectInitializer);

	static ULoadMapSystem* Get(const UObject* WorldContextObject);

	/* 外部调用加载Level，可以选择是异步加载还是同步加载 */
	void LoadLevel(const UObject* WorldContextObject , FString LevelName , bool BAsyn = true);

protected:
	/* 异步加载完成后的回调 */
	UFUNCTION()
	void AsynLoadPackageCall(const FName& PackageName, UPackage* LoadedPackage, int32 InResult);
};