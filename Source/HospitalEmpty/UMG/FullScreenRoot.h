// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "FullScreenRoot.generated.h"

/**
 * 
 */
UCLASS()
class UFullScreenRoot : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UFullScreenRoot(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel *Root;   //一定会有一个根节点！
};
