﻿#include "Sofa.h"
/*
#include "DecorationGridMgrComponent.h"
#include "GroundObj.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DtSceneComponentBase.h"
#include "Engine/StaticMesh.h"
#include "GroundUtil.h"*/

ASofa::ASofa(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	
}

void ASofa::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void ASofa::BeginPlay()
{
	Super::BeginPlay();
}

void ASofa::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
