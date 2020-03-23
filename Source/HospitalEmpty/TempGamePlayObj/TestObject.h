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

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "创建UI参数")
	FString CreateUIPath = "WidgetBlueprint'/Game/HospitalProject/Blueprints/UI/WBP_CreateUI.WBP_CreateUI_C'";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "创建UI参数")
	FString SaveUIPath = "WidgetBlueprint'/Game/HospitalProject/Blueprints/UI/WBP_SaveTestUI.WBP_SaveTestUI_C'";

protected:
	UPROPERTY()
	UUserWidgetBase* CreateWidget;

	UPROPERTY()
	UUserWidgetBase* SaveWidget;


};