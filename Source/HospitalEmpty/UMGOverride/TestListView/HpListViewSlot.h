#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "Components/Widget.h"
#include "SHpListView.h"
#include "Widgets/Layout/Anchors.h"
#include "HpListViewSlot.generated.h"

USTRUCT(BlueprintType)
struct FHPAnchorData
{
public:
	GENERATED_USTRUCT_BODY()

public:

	/** Offset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
	FMargin Offsets;

	/** Anchors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
	FAnchors Anchors;

	/**
	 * Alignment is the pivot point of the widget.  Starting in the upper left at (0,0),
	 * ending in the lower right at (1,1).  Moving the alignment point allows you to move
	 * the origin of the widget.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnchorData)
	FVector2D Alignment;

	/**
	* Compares this set of FHPAnchorData with another for equality.
	*
	* @param Other The other FHPAnchorData.
	* @return true if the two FAnchors are equal, false otherwise.
	*/
	bool operator==(const FHPAnchorData& Other) const
	{
		return Offsets == Other.Offsets &&
			Anchors == Other.Anchors &&
			Alignment == Other.Alignment;
	}

	/**
	* Compares this set of FHPAnchorData with another for inequality.
	*
	* @param Other The other FHPAnchorData.
	* @return true if the two FAnchors are not equal, false otherwise.
	*/
	bool operator!=(const FHPAnchorData& Other) const
	{
		return !(*this == Other);
	}
};

/*
	自定义ListView的Slot部分
*/
UCLASS(BlueprintType)
class UHpListViewSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()

public:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	/** Applies all properties to the live slot if possible. */
	virtual void SynchronizeProperties() override;

	void BuildSlot(TSharedRef<SHpListView> InHpListView);

#if WITH_EDITOR
	/* 用键盘左右移动时的回调 */
	virtual bool NudgeByDesigner(const FVector2D& NudgeDirection, const TOptional<int32>& GridSnapSize) override;
	/* 将小部件拖动到Canvas上时的回调 */
	virtual bool DragDropPreviewByDesigner(const FVector2D& LocalCursorPosition, const TOptional<int32>& XGridSnapSize, const TOptional<int32>& YGridSnapSize) override;
	virtual void SynchronizeFromTemplate(const UPanelSlot* const TemplateSlot) override;
#endif 

	/* Property变动相关，调用的是Object的PropertyChange接口 */
#if WITH_EDITOR
	using Super::PreEditChange;
	virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif

/* 设置属性相关 */
public:
	void SetBAutoSize(bool InAutoSize);
	bool GetBAutoSize();

	/* 设置Size */
	void SetOffsetSize(FVector2D InSize);
	/* 获取Size */
	FMargin GetOffsetSize();

	/* 虽然写了，但是并不在当前ListView里面实现，没必要 */
	void SetZorder(int32 InZOrder);
	int32 GetZorder() { return ZOrder; };

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Canvas Slot")
	FHPAnchorData LayoutData;

	/** The order priority this widget is rendered in.  Higher values are rendered last (and so they will appear to be on top). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Canvas Slot")
	int32 ZOrder;

	/* 是否自动大小，如果为True，则Slot大小取用Content Widget的大小，否则使用Slot的大小 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Canvas Slot")
	bool BAutoSize;


protected:
	SHpListView::FSlot* Slot;
};