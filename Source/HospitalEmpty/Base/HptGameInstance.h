#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HptGameInstance.generated.h"

UCLASS()
class UHptGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UHptGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	};

	static UHptGameInstance* GetInstance(const UObject* WorldContextObject);

	virtual void Init();
	virtual void Shutdown()override;


public:
	//获取格子管理器
	UFUNCTION(BlueprintCallable)
	class UDecorationSystemMgr* GetDecorationSystemMgr()
	{
		return DecorationSystemMgr;
	}
	UFUNCTION(BlueprintCallable)
	class UUMGManager* GetUMGManager()
	{
		return UMGManger;
	}
	UFUNCTION(BlueprintCallable)
	class USerializeSystemNew* GetSerializeSystem()
	{
		return SerializeSystem;
	}
	UFUNCTION(BlueprintCallable)
	class USaveGameSystem* GetSaveGameSystem()
	{
		return SaveGameSystem;
	}
	UFUNCTION(BlueprintCallable)
	class ULoadMapSystem* GetULoadMapSystem()
	{
		return LoadMapSystem;
	}

protected:

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	UPROPERTY()
	class UUMGManager* UMGManger;

	/* 序列化系统 */
	UPROPERTY()
	class USerializeSystemNew* SerializeSystem;

	/* 保存游戏系统 */
	UPROPERTY()
	class USaveGameSystem* SaveGameSystem;

	/* 加载地图系统 */
	UPROPERTY()
	class ULoadMapSystem* LoadMapSystem;
};
