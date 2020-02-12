UUIWidgetComponentBase:

		UUIWidgetComponentBase *temp_base = UUIWidgetComponentBase::CreateUIWidgetBase(tempMesh->GetStaticMeshComponent(), true,true);
		temp_touchUI = temp_base->SetUI<UTestTouchUI>("WidgetBlueprint'/Game/StoryTest/UI/TouchUITest.TouchUITest_C'");

静态方法UUIWidgetComponentBase::CreateUIWidgetBase可以创建在ACTOR上绑定一个UI ，tempMesh在此为从场景中获取的staticMesh物品 ，还有一些设置位置方法等


UUMGManager:
	CreateScreenWidget 方法可以创建一个直接显示在Viewport上的UI，有两种重载方法
	CreateInstanceWidget  方法可以在GameInstance中创建基于UFullScreenBse的不会随着场景释放的UI


FullScreenWidgetBase：
	其中只有一个Root节点，必须在UI蓝图中实现该节点（请修改改一开始的默认节点名）

FullScreenRoot为动态生成的Widget，不需要使用蓝图重新创建