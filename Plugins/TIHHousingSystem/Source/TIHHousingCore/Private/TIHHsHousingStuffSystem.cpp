// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsHousingStuffSystem.h"


#include "JsonObjectConverter.h"
#include "TIHHsSQLiteHelper.h"
#include "../Public/TIHComponents/TIHHsBaseRootComponent.h"
#include "../Public/TIHComponents/TIHHsMetaDataComponent.h"
#include "../Public/TIHComponents/TIHHsPlacementComponent.h"
#include "../Public/TIHComponents/TIHHsTriggerComponent.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "Kismet/GameplayStatics.h"

void UTIHHsHousingStuffSystem::CreateDatabaseIfNotExists(const FString& dbName, FString& prefabDBPath)
{
	prefabDBPath = FPaths::ProjectSavedDir() / dbName;

	//	dir check
	{
		FString prefabDir = FPaths::GetPath(prefabDBPath);
		if (!IFileManager::Get().DirectoryExists(*prefabDir))
		{
			IFileManager::Get().MakeDirectory(*prefabDir, /*Tree=*/true);
		}
	}
	
	if (not FPaths::FileExists(prefabDBPath))
	{
		// SaveStringToFile 없이 빈 파일 생성
		TArray<uint8> Empty;
		FFileHelper::SaveArrayToFile(Empty, *prefabDBPath);
		UE_LOG(LogTemp, Log, TEXT("Created empty DB file: %s"), *prefabDBPath);
	}
}

bool UTIHHsHousingStuffSystem::InitializeDatabase()
{
	FString prefabDBPath;
	
	CreateDatabaseIfNotExists(mConfigure.SaveDirDBName, prefabDBPath);

	mSQLiteHelper.Reset(NewObject<UTIHHsSQLiteHelper>());
	if (not mSQLiteHelper->Open(prefabDBPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("SQLite Open 실패: %s"), *prefabDBPath);
		return false;
	}

	EnsureDefinitionsTable();

	LoadDefinitionsToMemory();
	return true;
}

void UTIHHsHousingStuffSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeDatabase();
	
}

void UTIHHsHousingStuffSystem::EnsureDefinitionsTable()
{
	FTIHSQLiteTableBuilder Builder =
		FSQLiteQueryBuilder::CreateTable(TEXT("PrefabDefinitions"))
			.IfNotExists()
			.WithColumn(TEXT("Name"),           ETIHSQLiteDataType::Text,
						{ FSQLiteConstraints::NotNull(), FSQLiteConstraints::PrimaryKey() })
			.WithColumn(TEXT("ActorClassPath"), ETIHSQLiteDataType::Text,
						{ FSQLiteConstraints::NotNull() })
			.WithColumn(TEXT("ComponentsJson"), ETIHSQLiteDataType::Text);

	if (!mSQLiteHelper->ExecuteWrite(Builder))
	{
		UE_LOG(LogTemp, Error, TEXT("PrefabDefinitions 테이블 생성에 실패했습니다."));
	}
	
}

void UTIHHsHousingStuffSystem::LoadDefinitionsToMemory()
{
	// 1) 캐시 초기화
	mPrefabTable.Empty();

	// 2) SELECT 빌더 준비
	FTIHSQLiteSelectBuilder Builder =
		FSQLiteQueryBuilder::Select({ TEXT("Name"), TEXT("ActorClassPath"), TEXT("ComponentsJson") })
						  .From(TEXT("PrefabDefinitions"));

	// 3) Select 호출 (콜백으로 각 행 처리)
	bool bSuccess = mSQLiteHelper->Select(Builder,
		[this](const FSQLiteRowView& sqlRow)
		{
			FTIHHsPrefabDefinition prefabDef;
			prefabDef.Name = FName(*sqlRow.GetColumn<FString>(0));
			prefabDef.ActorClass = LoadClass<AActor>(nullptr, *sqlRow.GetColumn<FString>(1));
			FString JsonStr = sqlRow.GetColumn<FString>(2);
			if (!JsonStr.IsEmpty())
			{
				// 이 부분이 바로 JsonObjectStringToUStruct 호출부
				FJsonObjectConverter::JsonObjectStringToUStruct(
					JsonStr, &prefabDef, 0, 0, false
				);
			}
			mPrefabTable.Add(prefabDef.Name, prefabDef);
		}
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("PrefabDefinitions 조회 실패"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded %d prefab definitions"), mPrefabTable.Num());
	}
}

void UTIHHsHousingStuffSystem::EnsureComponentTagsTable()
{
	//	TODO: PrefabComponentTags 이거 바꿀수있게 변경
	// CREATE TABLE IF NOT EXISTS PrefabComponentTags (
	//   PrefabName   TEXT NOT NULL,
	//   ComponentTag TEXT
	// );
	FTIHSQLiteTableBuilder Builder =
		FSQLiteQueryBuilder::CreateTable(TEXT("PrefabComponentTags"))
			.IfNotExists()
			.WithColumn(TEXT("PrefabName"), ETIHSQLiteDataType::Text, { FSQLiteConstraints::NotNull() })
			.WithColumn(TEXT("ComponentTag"), ETIHSQLiteDataType::Text);
	if (not mSQLiteHelper->ExecuteWrite(Builder))
	{
		UE_LOG(LogTemp, Error, TEXT("PrefabComponentTags 테이블 생성에 실패했습니다."));
	}
}

void UTIHHsHousingStuffSystem::LoadComponentTagsToMemory()
{
}

void UTIHHsHousingStuffSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

}

FTIHHsPrefabDefinition UTIHHsHousingStuffSystem::CaptureActorAsPrefab(AActor* SourceActor)
{
	FTIHHsPrefabDefinition outDef;

	InitializeDatabase();

	return outDef;
}

