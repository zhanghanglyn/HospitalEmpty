#include "FHpListViewStyle.h"

FHpListViewStyle::FHpListViewStyle()
{

}

const FName FHpListViewStyle::TypeName(TEXT("FHpListViewStyle"));

const FHpListViewStyle& FHpListViewStyle::GetDefault()
{
	static FHpListViewStyle Default;
	return Default;
}

void FHpListViewStyle::GetResources(TArray<const FSlateBrush *> & OutBrushes) const
{
	OutBrushes.Add(&BackgroundImage);
}

FHpListViewStyle& FHpListViewStyle::SetBackgroundImage(const FSlateBrush& InBackgroundImage)
{
	BackgroundImage = InBackgroundImage;
	return *this;
}