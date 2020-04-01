#include "HpTestListView.h"
#include "Components/CanvasPanelSlot.h"

UHpTestListView::UHpTestListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FText const UHpTestListView::GetPaletteCategory()
{
	return FText::FromString(FString(L"自定义UMG"));  
}

void UHpTestListView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	HpListView.Reset();
}

UClass* UHpTestListView::GetSlotClass() const
{
	return UHpListViewSlot::StaticClass();
}

void UHpTestListView::OnSlotAdded(UPanelSlot* InSlot)
{
	if (HpListView.IsValid())
	{
		CastChecked<UHpListViewSlot>(InSlot)->BuildSlot(
			HpListView.ToSharedRef()
		);
	}
}

void UHpTestListView::OnSlotRemoved(UPanelSlot* InSlot)
{
	if (HpListView.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
			HpListView->RemoveSlot(Widget.ToSharedRef());
	}
}

UHpListViewSlot* UHpTestListView::AddChildToList(UWidget* Content)
{
	return Cast<UHpListViewSlot>(Super::AddChild(Content));
}

/************************************************************************/
/*                              显示相关                               */
/************************************************************************/
/*void UHpTestListView::PostLoad()
{
	Super::PostLoad();

	if (GetLinkerUE4Version() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS)
	{
		if (Style_DEPRECATED != nullptr)
		{
			const FHpListViewStyle* StylePtr = Style_DEPRECATED->GetStyle<FHpListViewStyle>();
			if (StylePtr != nullptr)
			{
				WidgetStyle = *StylePtr;
			}

			if (BackgroundImage_DEPRECATED != nullptr)
			{
				WidgetStyle.BackgroundImage = BackgroundImage_DEPRECATED->Brush;
				BackgroundImage_DEPRECATED = nullptr;
			}


			Style_DEPRECATED = nullptr;
		}
	}
}*/

void UHpTestListView::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SHpListView::FArguments SlateDefaults;
	HpListView.Get()->SetBackGroundImage(SlateDefaults._InBGImage.Get());
	HpListView.Get()->SetStyle(&WidgetStyle);
	HpListView.Get()->SetRow(Row);
	HpListView.Get()->SetBStartNotOffset(BStartNotOffset);
	HpListView.Get()->SetItemOffSet(ItemOffSet);
	HpListView.Get()->SetBClipping(BClipping);

	/* 获取DesignSize并且进行设置 */
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		DesignSize = CanvasPanelSlot->GetSize();
	}
	HpListView.Get()->SetDesignSize(DesignSize);
}

TSharedRef<SWidget> UHpTestListView::RebuildWidget()
{
	//在这里为他绑定上点击回调参数
	HpListView = SNew(SHpListView) //
		.InRow(5).InColumn(5).InBClipping(BClipping);//.InBGImage()

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UHpListViewSlot* TypedSlot = Cast<UHpListViewSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;

			TypedSlot->BuildSlot(HpListView.ToSharedRef());
		}
	}

	HpListView.Get()->SetMouseDownParam(nullptr);

	//用蓝图去进行事件的绑定，没有必要在这里设置了
	//HpListView.Get()->DelegateMouseBtnDownCall.BindUObject(this, &UHpTestListView::MouseDownListViewCall);

	return HpListView.ToSharedRef();
}

/*******************************************************************/
/*                        点击回调相关                             */
/*******************************************************************/
void UHpTestListView::BindMouseButtonDownCall(UObject* InObj, FName InFunctionName, FString Param)
{
	DelegateMouseButtonDown.BindUFunction(InObj, InFunctionName,Param);
}


void UHpTestListView::MouseDownListViewCall(const FGeometry&, const FPointerEvent&, UUMGParamBase* Param)
{
	UE_LOG(LogTemp, Warning, TEXT("  MouseDownListViewCall   !!!!!!!!!"));
	if (DelegateMouseButtonDown.IsBound())
		DelegateMouseButtonDown.Execute("1");
}

