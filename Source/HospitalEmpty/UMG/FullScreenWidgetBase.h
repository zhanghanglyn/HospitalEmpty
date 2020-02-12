// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "FullScreenWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class UFullScreenWidgetBase : public UUserWidgetBase
{
	GENERATED_BODY()
public:
	UFullScreenWidgetBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	
	//UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	//UCanvasPanel *Root;   //һ������һ�����ڵ㣡

};
