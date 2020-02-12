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

	virtual void Init();
	virtual void Shutdown()override;


public:
	//获取格子管理器
	class UDecorationSystemMgr* GetDecorationSystemMgr()
	{
		return DecorationSystemMgr;
	}

protected:

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

};
