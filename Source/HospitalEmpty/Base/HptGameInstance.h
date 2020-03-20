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
	class UDecorationSystemMgr* GetDecorationSystemMgr()
	{
		return DecorationSystemMgr;
	}

	class UUMGManager* GetUMGManager()
	{
		return UMGManger;
	}

	class USerializeSystemNew* GetSerializeSystem()
	{
		return SerializeSystem;
	}

protected:

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	UPROPERTY()
	class UUMGManager* UMGManger;

	/* 序列化系统 */
	UPROPERTY()
	class USerializeSystemNew* SerializeSystem;
};
