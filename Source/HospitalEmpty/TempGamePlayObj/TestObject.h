// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UMGManager.h"
#include "TestObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ATestObject : public AActor
{
	GENERATED_BODY()

public:
	ATestObject(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void LoadUI();

	UFUNCTION(BlueprintCallable)
	void OnClickedListItem0();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem1();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem2();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "创建UI参数")
	FString CreateUIPath = "WidgetBlueprint'/Game/StoryTest/UI/UI_NoticeDurationTip.UI_NoticeDurationTip_C'";

protected:
	UPROPERTY()
	UUserWidgetBase* CreateWidget;



};