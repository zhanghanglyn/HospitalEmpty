// Fill out your copyright notice in the Description page of Project Settings.


#include "UMGManager.h"

//UUMGManager* UUMGManager::Instace = nullptr;
UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	m_ScreenWidget.Empty();
}

UFullScreenWidgetBase* UUMGManager::CreateScreenWidget(const UObject* WorldContextObject, FString _widgetBlueprintPath, EUMGLayer Layer /* = EUMGLayer::None */, int32 _zorder /* = 0 */)
{
	/*if (m_ScreenWidget.Num() > 0 && m_ScreenWidget.Find(Layer) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("this name is aleady in the map!!!!"));
		return nullptr;
	}*/
	UClass* Temp_Widget = LoadClass<UFullScreenWidgetBase>(NULL, _widgetBlueprintPath.GetCharArray().GetData());
	if (Temp_Widget != nullptr)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		UFullScreenWidgetBase *NewWidget = CreateWidget<UFullScreenWidgetBase>(World, Temp_Widget);
		if (NewWidget != nullptr)
		{
			NewWidget->AddToViewport(((int8)Layer * 100 + _zorder));

			if (m_ScreenWidget.Contains(Layer))
				m_ScreenWidget[Layer].LayerWidgets.Add(NewWidget);
			else
			{
				FUMGLayerWidget LayerWidgets;
				LayerWidgets.LayerWidgets.Add(NewWidget);
				m_ScreenWidget.Add(Layer, LayerWidgets);
			}
			return NewWidget;
		}

		//m_ScreenWidget.Add(_widgetName, NewWidget);

		return nullptr;
	}

	return nullptr;
}

TArray<UFullScreenWidgetBase*> UUMGManager::GetScreenWidget(EUMGLayer Layer)
{
	TArray<UFullScreenWidgetBase*> ReturnArray;

	if (m_ScreenWidget.Find(Layer) != nullptr)
	{
		//FUMGLayerWidget* LayerWidget = m_ScreenWidget.Find(Layer);
		return m_ScreenWidget.Find(Layer)->LayerWidgets;
	}

	return ReturnArray;
}

UFullScreenWidgetBase* UUMGManager::GetScreenWidget(FString UID)
{
	for (auto& Widgets : m_ScreenWidget)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget->GetUID().Equals(UID))
			{
				return Widget;
			}
		}
	}
	return nullptr;
}

EUMGLayer UUMGManager::GetScreenWidgetLayer(UFullScreenWidgetBase* InWidget)
{
	for (auto& Widgets : m_ScreenWidget)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget == InWidget)
			{
				return Widgets.Key;
			}
		}
	}
	return EUMGLayer::None;
}

EUMGLayer UUMGManager::GetScreenWidgetLayer(FString UID)
{
	for (auto& Widgets : m_ScreenWidget)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget->GetUID().Equals(UID))
			{
				return Widgets.Key;
			}
		}
	}
	return EUMGLayer::None;
}

/*根据UID清除单个UMG*/
void UUMGManager::ClearWidget(FString UID)
{
	if (m_ScreenWidget.Num() > 0)
	{
		UFullScreenWidgetBase *tempWidget = GetScreenWidget(UID);//*m_ScreenWidget.Find(_widgetName);
		if (tempWidget)
		{
			EUMGLayer WidgetLayer = GetScreenWidgetLayer(UID);

			if (tempWidget->IsInViewport())
				tempWidget->RemoveFromViewport();

			m_ScreenWidget[WidgetLayer].LayerWidgets.Remove(tempWidget);
			tempWidget = nullptr;
			
		}
	}
}
/*根据Widget清除单个UMG*/
void UUMGManager::ClearWidget(UFullScreenWidgetBase* InWidget)
{
	ClearWidget(InWidget->GetUID());
}
/* 清除整个层上的UMG */
void UUMGManager::ClearWidget(EUMGLayer Layer)
{
	if (m_ScreenWidget.Num() > 0)
	{
		TArray<UFullScreenWidgetBase*> tempWidgetList = GetScreenWidget(Layer);//*m_ScreenWidget.Find(_widgetName);
		for (auto& Widgets : tempWidgetList)
		{
			if (Widgets->IsInViewport())
				Widgets->RemoveFromViewport();

			m_ScreenWidget[Layer].LayerWidgets.Remove(Widgets);
			Widgets = nullptr;
		}

		m_ScreenWidget.Remove(Layer);
	}
}

void UUMGManager::ClearAll()
{
	if (m_ScreenWidget.Num() > 0)
	{
		for (auto& Widgets : m_ScreenWidget)
		{
			for (UFullScreenWidgetBase* ShowWidget : Widgets.Value.LayerWidgets)
			{
				if (ShowWidget->IsInViewport())
				{
					ShowWidget->RemoveFromViewport();
				}
			}
		}
	}
	m_ScreenWidget.Empty();
	//m_WidgetTypeRelate.Empty();
}

void UUMGManager::SetInputMode(UWorld* _world, FInputModeDataBase& InData)
{
	if (_world != nullptr)
		_world->GetFirstPlayerController()->SetInputMode(InData);
}


/************************************************************************/
/*						创建至GameInstance相关                          */
/************************************************************************/
void UUMGManager::CreateInstanceRootWidget(UGameInstance * GameInstance)
{
	if (m_RootWidget != nullptr)
		return;

	//修改为不需要外部创建蓝图，直接床架哪一个UFullScreenRoot类型的UI，并且初始化根节点
		m_RootWidget = CreateWidget<UFullScreenRoot>(GameInstance->GetWorld(), UFullScreenRoot::StaticClass());
		if (m_RootWidget != nullptr)
		{
			//手动设置USERWIDGET的Root
			m_RootWidget->WidgetTree->RootWidget = m_RootWidget->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
			m_RootWidget->Root = Cast<UCanvasPanel>(m_RootWidget->WidgetTree->RootWidget);
			if (m_RootWidget->Root == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("UFullScreenRoot Add Root Error!!!!"));
			}

			GameInstance->GetGameViewportClient()->AddViewportWidgetContent(m_RootWidget->TakeWidget(), 0);
			//FVector2D Result;
			GameInstance->GetGameViewportClient()->GetViewportSize(m_ViewPortSize);
		}
	
}

//创建INstanceUMG
UFullScreenWidgetBase* UUMGManager::CreateInstanceWidget(const UObject* WorldContextObject, FString _widgetBlueprintPath,
	EUMGLayer Layer, int32 _zorder)
{
	if (m_RootWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("the m_RootWidget IS null!!!!!!!!!!!"));
		return nullptr;
	}
	if (m_RootWidget->Root == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("the m_RootWidget->Root IS null!!!!!!!!!!!"));
		return nullptr;
	}

	UClass* Temp_Widget = LoadClass<UFullScreenWidgetBase>(NULL, _widgetBlueprintPath.GetCharArray().GetData());
	if (Temp_Widget != nullptr)
	{
		UFullScreenWidgetBase *NewWidget = CreateWidget<UFullScreenWidgetBase>( Cast<UUserWidget>(m_RootWidget) , Temp_Widget);
		if (NewWidget != nullptr)
		{
			m_RootWidget->Root->AddChildToCanvas(NewWidget);
			UCanvasPanelSlot * Canvas = Cast <UCanvasPanelSlot>(NewWidget->Slot);
			if (Canvas != nullptr)
			{
				//如果动态修改了ViewPort，应该在此添加更新
				UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
				FVector2D Result;
				World->GetGameInstance()->GetGameViewportClient()->GetViewportSize(Result);
				Canvas->SetSize(Result);
			}

			//设置ZOrder
			NewWidget->SetZorder(((int8)Layer * 100 + _zorder));

			if (m_InsWidgetList.Contains(Layer))
				m_InsWidgetList[Layer].LayerWidgets.Add(NewWidget);
			else
			{
				FUMGLayerWidget LayerWidgets;
				LayerWidgets.LayerWidgets.Add(NewWidget);
				m_InsWidgetList.Add(Layer, LayerWidgets);
			}
		}

		//m_InsWidgetList.Push(NewWidget);

		return NewWidget;
	}

	return nullptr;
}

UFullScreenWidgetBase* UUMGManager::CreateInstanceWidget(UWorld* _world, FString _widgetBlueprintPath, 
	EUMGLayer Layer, int32 _zorder)
{
   	if (m_RootWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("the m_RootWidget IS null!!!!!!!!!!!"));
		return nullptr;
	}
	if (m_RootWidget->Root == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("the m_RootWidget->Root IS null!!!!!!!!!!!"));
		return nullptr;
	}

	UClass* Temp_Widget = LoadClass<UFullScreenWidgetBase>(NULL, _widgetBlueprintPath.GetCharArray().GetData());
	if (Temp_Widget != nullptr)
	{
		UFullScreenWidgetBase *NewWidget = CreateWidget<UFullScreenWidgetBase>(m_RootWidget, Temp_Widget);
		if (NewWidget != nullptr)
		{
			m_RootWidget->Root->AddChildToCanvas(NewWidget);
			UCanvasPanelSlot * Canvas = Cast <UCanvasPanelSlot>(NewWidget->Slot);
			if (Canvas != nullptr)
			{
				//如果动态修改了ViewPort，应该在此添加更新
				FVector2D Result;
				_world->GetGameInstance()->GetGameViewportClient()->GetViewportSize(Result);
				Canvas->SetSize(Result);
			}

			//设置ZOrder
			NewWidget->SetZorder(((int8)Layer * 100 + _zorder));

			if (m_InsWidgetList.Contains(Layer))
				m_InsWidgetList[Layer].LayerWidgets.Add(NewWidget);
			else
			{
				FUMGLayerWidget LayerWidgets;
				LayerWidgets.LayerWidgets.Add(NewWidget);
				m_InsWidgetList.Add(Layer, LayerWidgets);
			}
		}

		//m_InsWidgetList.Push(NewWidget);
		//记录一个INDEX与名字的关系
		//m_InsWidgetIndexList.Add(_widgetName, (m_InsWidgetList.Num() - 1));

		return NewWidget;
	}

	return nullptr;
}

UFullScreenWidgetBase* UUMGManager::GetInsUMG(FString UID)
{
	for (auto& Widgets : m_InsWidgetList)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget->GetUID().Equals(UID))
			{
				return Widget;
			}
		}
	}
	return nullptr;
}

TArray<UFullScreenWidgetBase*> UUMGManager::GetInsUMGS(EUMGLayer Layer)
{
	TArray<UFullScreenWidgetBase*> ReturnArray;

	if (m_InsWidgetList.Find(Layer) != nullptr)
	{
		return m_InsWidgetList.Find(Layer)->LayerWidgets;
	}

	return ReturnArray;
}

EUMGLayer UUMGManager::GetInsWidgetLayer(UFullScreenWidgetBase* InWidget)
{
	for (auto& Widgets : m_InsWidgetList)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget == InWidget)
			{
				return Widgets.Key;
			}
		}
	}
	return EUMGLayer::None;
}

EUMGLayer UUMGManager::GetInsWidgetLayer(FString UID)
{
	for (auto& Widgets : m_InsWidgetList)
	{
		for (auto& Widget : Widgets.Value.LayerWidgets)
		{
			if (Widget->GetUID().Equals(UID))
			{
				return Widgets.Key;
			}
		}
	}
	return EUMGLayer::None;
}


//删除全屏UMG
void UUMGManager::ClearInsWidget(FString UID)
{
	if (m_InsWidgetList.Num() > 0)
	{
		UFullScreenWidgetBase *tempWidget = GetScreenWidget(UID);
		if (tempWidget)
		{
			EUMGLayer WidgetLayer = GetScreenWidgetLayer(UID);

			tempWidget->RemoveFromViewport();

			m_InsWidgetList[WidgetLayer].LayerWidgets.Remove(tempWidget);
			tempWidget = nullptr;

		}
	}
}

//删除全屏UMG
void UUMGManager::ClearInsWidget(EUMGLayer Layer)
{
	if (m_InsWidgetList.Num() > 0)
	{
		TArray<UFullScreenWidgetBase*> tempWidgetList = GetScreenWidget(Layer);
		for (auto& Widgets : tempWidgetList)
		{
			Widgets->RemoveFromViewport();

			m_InsWidgetList[Layer].LayerWidgets.Remove(Widgets);
			Widgets = nullptr;
		}

		m_InsWidgetList.Remove(Layer);
	}
}

void UUMGManager::ClearInsWidget(UFullScreenWidgetBase* InWidget)
{
	ClearInsWidget(InWidget->GetUID());
}

void UUMGManager::ClearAllIns()
{
	if (m_InsWidgetList.Num() > 0)
	{
		for (auto& Widgets : m_InsWidgetList)
		{
			for (UFullScreenWidgetBase* ShowWidget : Widgets.Value.LayerWidgets)
			{
				ShowWidget->RemoveFromViewport();
			}
		}
	}
	m_InsWidgetList.Empty();
}