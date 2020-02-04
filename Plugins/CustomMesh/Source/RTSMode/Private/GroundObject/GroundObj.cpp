﻿#include "GroundObj.h"
#include "CustomMesh/Private/CustomWall/CustomWall.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GroundGridMgrComponent.h"

AGroundObj::AGroundObj(FString InActorName) : Super(InActorName)
{
	UE_LOG(LogTemp, Warning, TEXT("Is This Worked?"));
}

AGroundObj::AGroundObj(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 创建一个初始地面
	GroundMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GroundDefaultMesh"));
	//GroundMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("GroundDefaultMesh"));
	//GroundMeshComponent->RegisterComponent();
	//GroundMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
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

	//创建一个格子数据,之后应该改成读取配置
	/*if (GridMgr == nullptr)
	{
		GridMgr = NewObject<UGroundGridMgrComponent>(this, TEXT("GrigMgr"));
		GridMgr->RegisterComponent();
		//GridMgr->(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		GridMgr->InitGridData(GroundWidthHeight);
		GridMgr->InitGridStartLocation(GetActorLocation());
	}*/
}

void AGroundObj::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GridMgr == nullptr)
	{
		GridMgr = NewObject<UGroundGridMgrComponent>(this, TEXT("GrigMgr"));
		GridMgr->RegisterComponent();
		//GridMgr->(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		GridMgr->InitGridData(GroundWidthHeight);
		GridMgr->InitGridStartLocation(GetActorLocation());
		//为地面格子创建一个材质实例
		CreateMaterialInstance();
		//更新格子数
		UpdateMaterialParam();
	}
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
		FVector RelativeLocation = FVector(TouchLocation.X - GetRelativeStart().X, TouchLocation.Y - GetRelativeStart().Y, GetRelativeStart().Z);
		FGridData ttt;
		GridMgr->GetTouchGrid(RelativeLocation.GetAbs() , ttt);
		ttt.BeOccupy = true;
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
			GridMgr->InitGridData(GroundWidthHeight);
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
	if (MeshMaterial)
	{
		GridDynamicMaterial = UMaterialInstanceDynamic::Create(MeshMaterial, GroundMeshComponent);
		GroundMeshComponent->SetMaterial(0, GridDynamicMaterial);
	}

}