// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/UserWidgetBase.h"
#include "UMGOverride/TestListView/HpTestListView.h"
#include "CreateWidget.generated.h"

/**
	������Ʒ�Ĳ˵�����������ȡ��������ӵ������е���Ʒ��Ȼ����ʾ
	�����Ʒ�󣬻�ֱ�Ӵ���һ��
 */
UCLASS()
class UCreateWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	
public:
	UCreateWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override
	{
		Super::NativeTick(MyGeometry , InDeltaTime);
	};

public:

	UFUNCTION(BlueprintCallable)
	void AddToListView(UWidget* Content);

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickListView(FString Param, FString TestParam);
	
	//���Զ�Ӧ���3����ͬ�ļҾ�
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem0();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem1();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem2();
	UFUNCTION(BlueprintCallable)
	void OnClickedListItem3();

	//3.10 ���л��ò���
	void CreateSerializeObj();
	void SaveSerialize();
	void LoadSerialize();
	void CreateObjAndSerialize();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	UHpTestListView *ListView;   

	UPROPERTY()
	class UDecorationSystemMgr* DecorationSystemMgr;

	//���л�������
	UPROPERTY()
	class ATestSerializeObj* TestSerializeObj;
};
