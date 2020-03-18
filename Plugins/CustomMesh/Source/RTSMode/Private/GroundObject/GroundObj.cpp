#include "GroundObj.h"
#include "CustomMesh/Private/CustomWall/CustomWall.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GroundGridMgrComponent.h"
#include "Serialization/BufferArchive.h"
#include "Misc/FileHelper.h"
#include "Serializable.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryReader.h"

AGroundObj::AGroundObj(FString InActorName) : Super(InActorName)
{
	UE_LOG(LogTemp, Warning, TEXT("Is This Worked?"));
}

AGroundObj::AGroundObj(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 创建一个初始地面
	GroundMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GroundDefaultMesh"));
	SetRootComponent(GroundMeshComponent);
	
	if (!GroundMeshPath.IsEmpty())
	{
		UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
		if (GroundMesh)
		{
			GroundMeshComponent->SetStaticMesh(GroundMesh);
			FBoxSphereBounds MeshBounds = GroundMesh->GetBounds();
			FBox MeshBox = MeshBounds.GetBox();
			FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X , GroundWidthHeight.Y / MeshBox.GetSize().Y , 1);
			GroundMeshComponent->SetWorldScale3D(Scale);
		}
	}
}

void AGroundObj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GridMgr == nullptr)
	{
		GridMgr = NewObject<UGroundGridMgrComponent>(this, TEXT("GrigMgr"));
		GridMgr->RegisterComponent();
		//GridMgr->(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		GridMgr->InitGridStartLocation(GetTopRightLocation());
		GridMgr->InitGridData(GroundWidthHeight , this);
		
		//为地面格子创建一个材质实例
		CreateMaterialInstance();
		//更新格子数
		UpdateMaterialParam();
	}
}

void AGroundObj::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning , TEXT(" GroundName IS :: %s") , *GetName() );

	if (GridMgr)
	{
		GridMgr->InitGridStartLocation(GetTopRightLocation());
		GridMgr->InitGridData(GroundWidthHeight, this);
	}
	
	//3.11 尝试读取序列化的数据
	//LoadObjectFromFile("TTTTTT1");
}

#pragma optimize("",off)
void AGroundObj::StartTouch(FVector TouchLocation)
{
	//开始创建一个墙
	UWorld* World = this->GetWorld();
	if (World)
	{
		//FVector CreatePos = FVector(TouchLocation.X, TouchLocation.Y, TouchLocation.Z + 50);
		//ACustomWall* testWall = World->SpawnActor<ACustomWall>(ACustomWall::StaticClass(), CreatePos, FRotator(0, 0, 0));
		//测试计算一个立方体
		//testWall->CreateWall(FVector(50, 50, 50));

		//测试，获取点击的格子
		//取Actor的左上角
		//FVector RelativeLocation = FVector(TouchLocation.X - GetRelativeStart().X, TouchLocation.Y - GetRelativeStart().Y, GetRelativeStart().Z);
		//FGridData ttt;
		//GridMgr->GetTouchGrid(RelativeLocation.GetAbs() , ttt);
		//ttt.BeOccupy = true;
	}
}
#pragma optimize("",on)

void AGroundObj::TouchHold(FVector TouchLocation)
{

}

void AGroundObj::TouchEnd(FVector TouchLocation)
{

}

#pragma optimize("",off)
void AGroundObj::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//UE_LOG(LogTemp, Warning, TEXT("Property changed!!!!!!!!"));
	if (PropertyChangedEvent.MemberProperty == nullptr)
		return;

	if (PropertyChangedEvent.MemberProperty->GetName() == "GroundMeshPath")
	{
		if (!GroundMeshPath.IsEmpty())
		{
			UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
			if (GroundMesh)
			{
				GroundMeshComponent->SetStaticMesh(GroundMesh);
				FBoxSphereBounds MeshBounds = GroundMesh->GetBounds();
				FBox MeshBox = MeshBounds.GetBox();
				FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X, GroundWidthHeight.Y / MeshBox.GetSize().Y, 1);
				GroundMeshComponent->SetWorldScale3D(Scale);
			}
		}
	}
	//在地面宽高更新的时候，去更新格子数据并且更新Mesh的材质参数
	else if(PropertyChangedEvent.MemberProperty->GetName() == "GroundWidthHeight")
	{
		if (GroundMeshComponent)
		{
			if (UStaticMesh* TempStaticMesh = GroundMeshComponent->GetStaticMesh())
			{
				FBoxSphereBounds MeshBounds = TempStaticMesh->GetBounds();
				FBox MeshBox = MeshBounds.GetBox();
				FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X, GroundWidthHeight.Y / MeshBox.GetSize().Y, 1);
				GroundMeshComponent->SetWorldScale3D(Scale);
			}
		}
		if (GridMgr)
		{
			GridMgr->InitGridData(GroundWidthHeight,this);
			//更新格子数
			UpdateMaterialParam();
		}
	}
	//在地面宽高更新的时候，去更新格子数据并且更新Mesh的材质参数
	else if (PropertyChangedEvent.MemberProperty->GetName() == "BorderWidth")
	{
		if (GridDynamicMaterial)
			GridDynamicMaterial->SetScalarParameterValue("BorderWidth", BorderWidth);
	}
}

FVector AGroundObj::GetRelativeStart()
{
	FVector Location = GetActorLocation();
	return FVector(Location.X - GroundWidthHeight.X/2 , Location.Y - GroundWidthHeight.Y/2, Location.Z);
}

#pragma optimize("",on)

void AGroundObj::UpdateMaterialParam()
{
	if (GridMgr)
	{
		MaterialParam.RowNum = GridMgr->GetGridRow();
		MaterialParam.Column = GridMgr->GetGridColumn();
		MaterialParam.BorderWidth = BorderWidth;
	}

	if (GridDynamicMaterial)
	{
		GridDynamicMaterial->SetScalarParameterValue("RowNum", MaterialParam.RowNum);
		GridDynamicMaterial->SetScalarParameterValue("Column", MaterialParam.Column);
		GridDynamicMaterial->SetScalarParameterValue("BorderWidth", MaterialParam.BorderWidth);
	}
}

void AGroundObj::CreateMaterialInstance()
{
	if (!GroundMeshComponent)
		return;
	UMaterialInterface* MeshMaterial = GroundMeshComponent->GetMaterial(0);

	//如果不是动态材质，则为其设置一个动态材质
	/*if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshMaterial))
	{
		DynamicMaterial->SetScalarParameterValue("RowNum", MaterialParam.RowNum);
		DynamicMaterial->SetScalarParameterValue("Column", MaterialParam.Column);
		DynamicMaterial->SetScalarParameterValue("BorderWidth", MaterialParam.BorderWidth);
	}
	else
	{
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(MeshMaterial, GroundMeshComponent);
		if (MaterialInstance)
		{
			GroundMeshComponent->SetMaterial(0, MaterialInstance);
			MaterialInstance->SetScalarParameterValue("RowNum", MaterialParam.RowNum);
			MaterialInstance->SetScalarParameterValue("Column", MaterialParam.Column);
			MaterialInstance->SetScalarParameterValue("BorderWidth", MaterialParam.BorderWidth);
		}
	}*/
	if (GridDynamicMaterial)
	{
		GridDynamicMaterial = nullptr;
	}
	if (GridMaterial)
	{
		GridDynamicMaterial = UMaterialInstanceDynamic::Create(GridMaterial, GroundMeshComponent);
		GroundMeshComponent->SetMaterial(0, GridDynamicMaterial);
	}

}

void AGroundObj::GetGridWidthHeight(float &Width, float &Height)
{
	FVector Scale = GroundMeshComponent->GetComponentTransform().GetScale3D();
	FBoxSphereBounds MeshBounds = GroundMeshComponent->GetStaticMesh()->GetBounds();
	Width = MeshBounds.GetBox().GetSize().X * Scale.X;
	Height = MeshBounds.GetBox().GetSize().Y * Scale.Y;
}

#pragma optimize("",off)
FVector AGroundObj::GetTopRightLocation()
{
	float Width = 0;
	float Height = 0;
	GetGridWidthHeight(Width, Height);
	FVector DecorationLocation = GetActorLocation();

	return FVector(DecorationLocation.X - Width / 2, DecorationLocation.Y - Height / 2, DecorationLocation.Z);
}

FVector AGroundObj::GetButtomLeftLocation()
{
	float Width = 0;
	float Height = 0;
	GetGridWidthHeight(Width, Height);
	FVector DecorationLocation = GetActorLocation();

	return FVector(DecorationLocation.X + Width / 2, DecorationLocation.Y + Height / 2, DecorationLocation.Z);
}

#pragma optimize("",on)

bool AGroundObj::CheckLocationInGround(FVector &InLocation)
{
	FVector ActorLocation = GetActorLocation();
	float Width;
	float Height;
	GetGridWidthHeight(Width, Height);

	bool Result = false;
	
	FVector2D RangeX = FVector2D(ActorLocation.X - Width / 2, ActorLocation.X + Width / 2);
	FVector2D RangeY = FVector2D(ActorLocation.Y - Height / 2, ActorLocation.Y + Height / 2);

	//先判断
	if (InLocation.X >= RangeX.X && InLocation.X <= RangeX.Y)
	{
		if (InLocation.Y >= RangeY.X && InLocation.Y <= RangeY.Y)
			Result = true;
	}

	//if (InLocation.X >= RangeX.X && InLocation.X <= RangeX.Y)
	{
		if (InLocation.X < RangeX.X)
			InLocation.X = RangeX.X;
		if (InLocation.X > RangeX.Y)
			InLocation.X = RangeX.Y;
	}
	//if (InLocation.Y >= RangeY.X && InLocation.Y <= RangeY.Y)
	{
		if (InLocation.Y < RangeY.X)
			InLocation.Y = RangeY.X;
		if (InLocation.Y > RangeY.Y)
			InLocation.Y = RangeY.Y;
	}

	return Result;

}

bool AGroundObj::CheckLeftBUttomLocationInGround(FVector &LeftButtomLocation)
{
	FVector ActorLocation = GetActorLocation();
	float Width;
	float Height;
	GetGridWidthHeight(Width, Height);

	FVector2D RangeX = FVector2D(ActorLocation.X - Width / 2, ActorLocation.X + Width / 2);
	FVector2D RangeY = FVector2D(ActorLocation.Y - Height / 2, ActorLocation.Y + Height / 2);

	bool Result = false;

	if (LeftButtomLocation.X >= RangeX.X && LeftButtomLocation.X <= RangeX.Y)
	{
		if (LeftButtomLocation.Y >= RangeY.X && LeftButtomLocation.Y <= RangeY.Y)
			Result = true;
	}
	//判断完毕后设置
	{
		if (LeftButtomLocation.X < RangeX.X)
			LeftButtomLocation.X = RangeX.X;
		if (LeftButtomLocation.X > RangeX.Y)
			LeftButtomLocation.X = RangeX.Y;
	}
	{
		if (LeftButtomLocation.Y < RangeY.X)
			LeftButtomLocation.Y = RangeY.X;
		if (LeftButtomLocation.Y > RangeY.Y)
			LeftButtomLocation.Y = RangeY.Y;
	}

	return Result;
}


bool AGroundObj::SaveCurDecoration(ADecorationBase* SaveDecoration)
{
	return GridMgr->SaveCurDecoration(SaveDecoration);
}

void AGroundObj::DeleteDecoration(ADecorationBase* DelDecoration)
{
	GridMgr->DeleteDecoration(DelDecoration);
}

/************************************************************************/
/*                             序列化相关                               */
/************************************************************************/
void AGroundObj::Serialize(FArchive& Ar)
{
	//if (Ar.IsSaving())
	//{
	//	if (GridMgr != nullptr)
	//		Ar << GridMgr;
	//}
	//else
	//	Ar << GridMgr;

	//Ar << MaterialParam;

	Super::Serialize(Ar);

	/* 应该是要手动序列化完毕之后进行初始化吧 */
}

void AGroundObj::SaveOrLoadData(FArchive& Ar)
{
	return;

	if (Ar.IsSaving())
		UE_LOG(LogTemp, Warning, TEXT("AGroundObj::SaveOrLoadData   !! Saving!"));
	if (Ar.IsLoading())
		UE_LOG(LogTemp, Warning, TEXT("AGroundObj::SaveOrLoadData   !! Loading!!!"));

	GroundMeshComponent->Serialize(Ar);

	Ar << GroundMeshPath;

	Ar << GroundWidthHeight;

	Ar << BorderWidth;

	Ar << GridMaterial;

	//Ar << GridMgr;

	//Ar << MaterialParam;

	//Ar << GridDynamicMaterial;
}

bool AGroundObj::SaveObjectToFile(FString FilePath)
{
	FBufferArchive ToBinary;
	SaveOrLoadData(ToBinary);

	if (ToBinary.Num() <= 0)
	{
		UE_LOG(LogTemp , Warning , TEXT("地面保存失败！没有数据要写！"));
		return false;
	}
		
	if (!FFileHelper::SaveArrayToFile(ToBinary, *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *FilePath)))
	{
		UE_LOG(LogTemp, Warning, TEXT("地面保存写出到文件失败！"));
		return false;
	}

	ToBinary.FlushCache();
	ToBinary.Empty();

	return true;
}

bool AGroundObj::LoadObjectFromFile(FString FilePath)
{
	return false;

	TArray<uint8> BinaryArray;
	if (!FFileHelper::LoadFileToArray(BinaryArray, *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *FilePath)))
	{
		UE_LOG(LogTemp, Warning, TEXT("地面读取文件失败！"));
		return false;
	}

	if (BinaryArray.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("地面读取文件内容为空！"));
		return false;
	}
	//true表明内容为可持续的
	FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
	FromBinary.Seek(0);

	SaveOrLoadData(FromBinary);

	FromBinary.FlushCache();
	BinaryArray.Empty();
	FromBinary.Close();

	return true;
}