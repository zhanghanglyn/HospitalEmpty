#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFrame/SaveGameSystem/SaveableActorInterface.h"
#include "HCharacterBase.generated.h"

/*
	场景中所有物体的基类，该物体有一个自定义的名字，并且响应Touch的三个阶段
*/
UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "所有当前游戏所有Character类的基类"))
class AHCharacterBase : public ACharacter
{
	GENERATED_BODY()
public:
	AHCharacterBase( FString InActorName );

	/** Constructor for AActor that takes an ObjectInitializer for backward compatibility */
	AHCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

	/* 开始被点击 */
	virtual void StartTouch(FVector TouchLocation) {};
	virtual void TouchHold(FVector TouchLocation) {};
	virtual void TouchEnd(FVector TouchLocation) {};
	virtual void Hover(FVector TouchLocation) {};
public:
	UPROPERTY(EditAnywhere, meta = (Category = "ActorBase"))
	FString CharacterName;

	/* 20.3.13 新添加是否用来进行保存 */
	UPROPERTY()
	int VariableToSave;
};
