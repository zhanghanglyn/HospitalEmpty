#pragma once

#include "CoreMinimal.h"
#include "SHpListView.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/PanelWidget.h"
#include "HpListViewSlot.h"
#include "HpListViewWidgetStyle.h"
#include "UMGOverride/OverrideUMGUtil.h"
#include "HpTestListView.generated.h"

/*
	测试自定义ListView UMG部分
*/
UCLASS(BlueprintType)
class UHpTestListView : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	/* 返回一个在UMG对应列表中的分类 */
	virtual const FText GetPaletteCategory() override;
	// UWidget interface
	virtual bool LockToPanelOnDrag() const override
	{
		return true;
	}
	// End UWidget interface
#endif
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	/** 
		Function implemented by all subclasses of UWidget is called when the underlying SWidget needs to be constructed.
		创建Slate在此函数中
	**/
	virtual void SynchronizeProperties() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	/* virtual end */

	//~ Begin UObject Interface
	//virtual void PostLoad() override;
	//~ End UObject Interface

	/* UPanelWidget Virtual */
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* InSlot);
	virtual void OnSlotRemoved(UPanelSlot* InSlot);

public:
	/* 外部调用，添加Item到List中 */
	UFUNCTION(BlueprintCallable, Category = "Canvas Panel")
	UHpListViewSlot* AddChildToList(UWidget* Content);

	/* 点击ListView的回调 */
	void MouseDownListViewCall(const FGeometry&, const FPointerEvent&, UUMGParamBase* Param);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ListView参数")
	FMargin ContentMargin;

	/* Offset */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ListView参数" , meta = (DisplayName = "横纵间距"))
	FVector2D ItemOffSet = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ListView参数", meta = (DisplayName = "横向ITEM数"))
	int32 Row = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ListView参数", meta = (DisplayName = "是否ITEM起始不加Offset"))
	bool BStartNotOffset = false; 

protected:
	/* 对应的SlistView Widget */
	TSharedPtr<SHpListView> HpListView;

	//UPROPERTY()
	//USlateWidgetStyleAsset* Style_DEPRECATED;

	/** 背景图ListView */
	//UPROPERTY()
	//USlateBrushAsset* BackgroundImage_DEPRECATED;

	/** The progress bar ex style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (DisplayName = "Style"))
	FHpListViewStyle WidgetStyle;
};