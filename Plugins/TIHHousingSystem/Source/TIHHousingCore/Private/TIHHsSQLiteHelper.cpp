// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsSQLiteHelper.h"

UTIHHsSQLiteHelper::UTIHHsSQLiteHelper()
	: mIsOpen(false)
{
	
}

UTIHHsSQLiteHelper::~UTIHHsSQLiteHelper()
{
	Close();
}

bool UTIHHsSQLiteHelper::Open(const FString& databasePath)
{
	if (mIsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("Database is already open. Closing it before opening a new one."));
		Close();
	}
	sqlite3* target = nullptr;
	int32 rc = sqlite3_open(TCHAR_TO_UTF8(*databasePath),&target );
	
	if (rc != SQLITE_OK)
	{
		FString errorMsg = target ? UTF8_TO_TCHAR(sqlite3_errmsg(target)) :
		TEXT("Unknown error");
		UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s. SQLite Error: %s (Code: %d)"), *databasePath, *errorMsg, rc);
		if (target)
		{
			sqlite3_close(target);
		}
		mDataBaseWrapper.SQLitePtr = nullptr;
		return false;
	}
	mDataBaseWrapper.SQLitePtr = target;
	mIsOpen = true;
	UE_LOG(LogTemp, Log, TEXT("Database opened successfully: %s"), *databasePath);
	
	return true;
}

bool UTIHHsSQLiteHelper::Close()
{
	if (mIsOpen && mDataBaseWrapper.IsValid())
	{
		mDataBaseWrapper.Close();
		mIsOpen = false;
		UE_LOG(LogTemp, Log, TEXT("Database closed successfully."));
		return true;
	}
	return false;
}

bool UTIHHsSQLiteHelper::Execute_deprecated(const FString& query)
{
	if (not mIsOpen)
	{
		UE_LOG(LogTemp, Error, TEXT("Database is not open. Cannot execute query: %s"), *query);
		return false;
	}
	//	 int  (*exec)(sqlite3*,const char*,sqlite3_callback,void*,char**)
	CHAR* erroMsg = nullptr;
	
	int32 rc = sqlite3_exec(
		mDataBaseWrapper.SQLitePtr,
		TCHAR_TO_UTF8(*query),
		nullptr, nullptr,
		&erroMsg);
	if (rc != SQLITE_OK)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to execute query: %s. Error: %s"), *query, UTF8_TO_TCHAR(erroMsg));
		sqlite3_free(erroMsg);
		return false;	
	}
	UE_LOG(LogTemp, Log, TEXT("Query executed successfully: %s"), *query);
	return true;
}

bool UTIHHsSQLiteHelper::PrepareStatement_deprecated(const FString& query, FTIHHsSQLiteStmtWrapper& statement)
{
	if (!mIsOpen )
	{
		UE_LOG(LogTemp, Error, TEXT("Database is not open. Cannot prepare statement: %s"), *query);
		return false;
	}
	int rc = sqlite3_prepare_v2(
		mDataBaseWrapper.SQLitePtr,
		TCHAR_TO_UTF8(*query),
		-1,
		&statement.SQLiteStmt,
		nullptr);
	if (rc != SQLITE_OK)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s. Error: %s"), *query, UTF8_TO_TCHAR(sqlite3_errmsg(mDataBaseWrapper.SQLitePtr)));
		return false;
	}
	return true;
}

void UTIHHsSQLiteHelper::FinalizeStatement_deprecated(FTIHHsSQLiteStmtWrapper& statement)
{
	if (statement.SQLiteStmt)
	{
		sqlite3_finalize(statement.SQLiteStmt);
	}
}

void UTIHHsSQLiteHelper::BeginDestroy()
{
	UObject::BeginDestroy();
	if (mIsOpen)
	{
		Close();
	}
}

bool UTIHHsSQLiteHelper::ExecuteWrite(const FTIHSQLiteTableBuilder& Builder)
{
	FString Query = const_cast<FTIHSQLiteTableBuilder&>(Builder).Build();
	// CREATE는 바인딩할 값이 없으므로 바로 Execute
	return Execute_deprecated(Query); // Execute는 내부적으로 sqlite3_exec 사용
}

bool UTIHHsSQLiteHelper::ExecuteWrite(const FTIHSQLiteInsertBuilder& Builder)
{
	FString queryString;
	TArray<FTIHSQLiteBindValue> bindValues;
	const_cast<FTIHSQLiteInsertBuilder&>(Builder).Build(queryString, bindValues);
    
	sqlite3_stmt* Stmt;
	if (PrepareStatement(queryString, Stmt) && BindValues(Stmt, bindValues))
	{
		bool bSuccess = (sqlite3_step(Stmt) == SQLITE_DONE);
		FinalizeStatement(Stmt);
		return bSuccess;
	}
	return false;
}

bool UTIHHsSQLiteHelper::ExecuteWrite(const FTIHSQLiteUpdateBuilder& Builder)
{
	FString queryString;
	TArray<FTIHSQLiteBindValue> bindValues;
	const_cast<FTIHSQLiteUpdateBuilder&>(Builder).Build(queryString, bindValues);
	
	sqlite3_stmt* Stmt;
	if (PrepareStatement(queryString, Stmt) && BindValues(Stmt, bindValues))
	{
		bool bSuccess = (sqlite3_step(Stmt) == SQLITE_DONE);
		FinalizeStatement(Stmt);
		return bSuccess;
	}
	return false;
}

bool UTIHHsSQLiteHelper::ExecuteRawWrite(const FString& query)
{
	if (not mIsOpen)
	{
		UE_LOG(LogTemp, Error, TEXT("Database is not open. Cannot execute raw query: %s"), *query);
		return false;
	}
	char* erroMsg = nullptr;

	int32 rc = sqlite3_exec(
		mDataBaseWrapper.SQLitePtr,
		TCHAR_TO_UTF8(*query),
		nullptr, nullptr,
		&erroMsg);
	
	if (rc != SQLITE_OK)
	{
		FString errorString = erroMsg ? UTF8_TO_TCHAR(erroMsg) : TEXT("Unknown error");
		UE_LOG(LogTemp, Error, TEXT("Failed to execute raw query: %s. SQLite Error: %s (Code: %d)"), 
			   *query, *errorString, rc);
        
		if (erroMsg)
		{
			sqlite3_free(erroMsg);
		}
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Raw query executed successfully: %s"), *query);
	return true;
}

bool UTIHHsSQLiteHelper::ExecuteRawRead(const FString& query, TFunction<void(const FSQLiteRowView& Row)> onRow)
{
	if (not mIsOpen)
	{
		UE_LOG(LogTemp, Error, TEXT("Database is not open. Cannot execute query: %s"), *query);
		return false;
	}

	sqlite3_stmt* stmt;
	if (not PrepareStatement(query, stmt))
	{
		return false;
	}

	FSQLiteRowView rowView(stmt);
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		onRow(rowView);
	}

	FinalizeStatement(stmt);
	return true;
}

bool UTIHHsSQLiteHelper::Select(const FTIHSQLiteSelectBuilder& Builder, TFunction<void(const FSQLiteRowView& Row)> OnRow)
{
	FString queryString;
	TArray<FTIHSQLiteBindValue> bindValues;
	const_cast<FTIHSQLiteSelectBuilder&>(Builder).Build(queryString, bindValues);

	sqlite3_stmt* Stmt;
	if (PrepareStatement(queryString, Stmt) && BindValues(Stmt, bindValues))
	{
		FSQLiteRowView RowView(Stmt);
		while (sqlite3_step(Stmt) == SQLITE_ROW)
		{
			OnRow(RowView); // 각 행에 대해 콜백 실행
		}
		FinalizeStatement(Stmt);
		return true;
	}
	return false;
}

void UTIHHsSQLiteHelper::ExampleCode0()
{
	// TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
	// FString dbPath = FPaths::ProjectConfigDir() / TEXT("example.db");
	// if (sqliteHelper->Open(dbPath) == true)
	// {
	// 	//	create a table if it does not exist
	// 	FString createTableQuery = TEXT("CREATE TABLE IF NOT EXISTS Player (ID INTEGER PRIMARY KEY,Name TEXT, Score REAL);");
	// 	sqliteHelper->Execute_deprecated(createTableQuery);
	//
	// 	//	insert data into the table
	// 	FTIHHsSQLiteStmtWrapper stmt;
	// 	FString insertQuery = TEXT("INSERT INTO Player (Name, Score) VALUES (?, ?);");
	//
	// 	if (sqliteHelper->PrepareStatement_deprecated(insertQuery,stmt))
	// 	{
	// 		//	bind parameters
	// 		sqlite3_bind_text(stmt.SQLiteStmt,1,"PlayerOne",-1,SQLITE_TRANSIENT);
	// 		sqlite3_bind_double(stmt.SQLiteStmt,2,100.0);
	// 		sqlite3_step(stmt.SQLiteStmt);
	// 		sqlite3_reset(stmt.SQLiteStmt);
	//
	// 		//	second data bind
	// 		sqlite3_bind_text(*stmt,1,"PlayerTwo",-1,SQLITE_TRANSIENT);
	// 		sqlite3_bind_double(*stmt,2,200.0);
	// 		sqlite3_step(*stmt);
	//
	// 		// finalize the statement
	// 		sqliteHelper->FinalizeStatement_deprecated(stmt);
	// 	}
	// 	sqliteHelper->Close();
	// }
	//
}

void UTIHHsSQLiteHelper::ExampleCode1()
{
	UE_LOG(LogTemp, Log, TEXT("ExampleCode1: Creating a table with various constraints."));
	FString Query = FSQLiteQueryBuilder::CreateTable("GameSettings")
		.IfNotExists()
		.WithColumn("Key", ETIHSQLiteDataType::Text, { FSQLiteConstraints::PrimaryKey() })
		.WithColumn("Value", ETIHSQLiteDataType::Text)
		.WithColumn("Priority", ETIHSQLiteDataType::Integer, { 
			FSQLiteConstraints::NotNull(), 
			FSQLiteConstraints::Default(100) // 정수형 기본값
		})
		.WithColumn("DefaultUserName", ETIHSQLiteDataType::Text, {
			FSQLiteConstraints::Default(TEXT("Guest")) // 문자열 기본값
		})
		.Build();

	// 생성된 쿼리:
	// CREATE TABLE IF NOT EXISTS GameSettings (
	//     Key TEXT PRIMARY KEY, 
	//     Value TEXT, 
	//     Priority INTEGER NOT NULL DEFAULT 100,
	//     DefaultUserName TEXT DEFAULT 'Guest'
	// );
	UE_LOG(LogTemp, Log, TEXT("Query: %s"), *Query);

}

void UTIHHsSQLiteHelper::ExampleCode2_Insert()
{
	TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
	FString dbPath = FPaths::ProjectConfigDir() / TEXT("game.db");
    
	if (sqliteHelper->Open(dbPath))
	{
		// 1. 단일 레코드 삽입,빌더를 사용함
		auto insertBuilder = FSQLiteQueryBuilder::InsertInto("Player")
			.Value("Name", TEXT("PlayerOne"))
			.Value("Score", 1500)
			.Value("Level", 25)
			.Value("IsActive", true);
            
		if (sqliteHelper->ExecuteWrite(insertBuilder))
		{
			UE_LOG(LogTemp, Log, TEXT("Player inserted successfully"));
		}
        
		// 2. 다양한 데이터 타입 삽입
		FVector playerPosition(100.0f, 200.0f, 50.0f);
		TArray<uint8> playerData = {0x01, 0x02, 0x03, 0x04};
        
		auto insertBuilder2 = FSQLiteQueryBuilder::InsertInto("GameData")
			.Value("PlayerId", 1)
			.Value("Position", playerPosition)  // FVector는 문자열로 변환됨
			.Value("BinaryData", playerData)    // BLOB 데이터
			.Value("CreatedAt", FDateTime::Now().ToString());
            
		sqliteHelper->ExecuteWrite(insertBuilder2);
        
		sqliteHelper->Close();
	}
	/*
	 *	매크로를 사용한 방법
		auto query = TIHSQL_INSERT("Users")
		.Value("Username", TEXT("NewUser"))
		.Value("Email", TEXT("user@example.com"))
		.Value("Age", 25);
	 */
}

void UTIHHsSQLiteHelper::ExampleCode3_Select()
{
	TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
	FString dbPath = FPaths::ProjectConfigDir() / TEXT("game.db");
    
	if (sqliteHelper->Open(dbPath))
	{
		// 1. 모든 플레이어 정보 조회
		auto selectQuery = FSQLiteQueryBuilder::Select({"Name", "Score", "Level"})
			.From("Player")
			.OrderBy("Score", ETIHSQLiteOrderBy::Descending);
            
		sqliteHelper->Select(selectQuery, [](const FSQLiteRowView& Row)
		{
			FString playerName = Row.GetColumn<FString>(0);  // Name
			int32 score = Row.GetColumn<int32>(1);           // Score  
			int32 level = Row.GetColumn<int32>(2);           // Level
            
			UE_LOG(LogTemp, Log, TEXT("Player: %s, Score: %d, Level: %d"), 
				*playerName, score, level);
		});
        
		sqliteHelper->Close();
	}
}

bool UTIHHsSQLiteHelper::PrepareStatement(const FString& query, sqlite3_stmt*& OutStmt) const
{
	if (not mIsOpen)
	{
		UE_LOG(LogTemp, Error, TEXT("Database is not open. Cannot prepare statement: %s"), *query);
		return false;
	}
	int rc  = sqlite3_prepare_v2(
		mDataBaseWrapper.SQLitePtr,
		TCHAR_TO_UTF8(*query),
		-1,
		&OutStmt,
		nullptr);
	
	if (rc != SQLITE_OK)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to prepare statement: %s. Error: %s"), *query, UTF8_TO_TCHAR(sqlite3_errmsg(mDataBaseWrapper.SQLitePtr)));
		return false;
	}
	return true;
}

void UTIHHsSQLiteHelper::FinalizeStatement(sqlite3_stmt* Stmt)
{
	if (Stmt)
	{
		sqlite3_finalize(Stmt);
	}
}

bool UTIHHsSQLiteHelper::BindValues(sqlite3_stmt* stmt, const TArray<FTIHSQLiteBindValue>& BindValues)
{
	if (stmt == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Statement is null. Cannot bind values."));
		return false;
	}
	for (int32 i = 0; i < BindValues.Num(); ++i)
	{
		const FTIHSQLiteBindValue& bindValue = BindValues[i];
		const int32 bindIndex = i + 1; // SQLite uses 1-based indexing for parameters
		int32 rc = SQLITE_OK;
		
		switch (bindValue.GetType())
		{
		case EVariantTypes::Empty:
			{
				UE_LOG(LogTemp, Warning, TEXT("SQLite: Unsupported bind type at index %d."), bindIndex);
				sqlite3_bind_null(stmt, bindIndex);
			}break;
		case EVariantTypes::Ansichar:
			{}break;
		case EVariantTypes::Bool:
			{
				rc = sqlite3_bind_int(stmt, bindIndex, (bindValue.Value.GetValue<bool>() ? 1:0));
			}break;
		case EVariantTypes::Box:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FBox>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::BoxSphereBounds:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FBoxSphereBounds>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::ByteArray:
			{
				const TArray<uint8>& byteArray = bindValue.Value.GetValue<TArray<uint8>>();
				rc = sqlite3_bind_blob(
					stmt, 
					bindIndex, 
					byteArray.GetData(),      // 데이터의 시작 포인터
					byteArray.Num(),          // 데이터의 크기 (바이트)
					SQLITE_TRANSIENT      // SQLite가 알아서 복사하도록 설정
				);
			}break;
		case EVariantTypes::Color:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FColor>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::DateTime:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FDateTime>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Double:
			{
				rc = sqlite3_bind_double(stmt, bindIndex, bindValue.Value.GetValue<double>());
			}break;
		case EVariantTypes::Enum:
			{
				//rc = sqlite3_bind_int(stmt, bindIndex, bindValue.Value.GetValue<Enum>());
			}break;
		case EVariantTypes::Float:
			{
				rc = sqlite3_bind_double(stmt, bindIndex, bindValue.Value.GetValue<float>());
			}break;
		case EVariantTypes::Guid:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FGuid>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Int8:
			{
				rc = sqlite3_bind_int(stmt, bindIndex, bindValue.Value.GetValue<int8>());
			}break;
		case EVariantTypes::Int16:
			{
				rc = sqlite3_bind_int(stmt, bindIndex, bindValue.Value.GetValue<int16>());
			}break;
		case EVariantTypes::Int32:
			{
				rc = sqlite3_bind_int(stmt, bindIndex, bindValue.Value.GetValue<int32>());
			}break;
		case EVariantTypes::Int64:
			{
				rc = sqlite3_bind_int64(stmt, bindIndex, bindValue.Value.GetValue<int64>());
			}break;
		case EVariantTypes::IntRect:
			{
				//rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FIntRect<int32>>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::LinearColor:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FLinearColor>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Matrix:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FMatrix>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Name:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FName>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Plane:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FPlane>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Quat:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FQuat>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::RandomStream:
			{}break;
		case EVariantTypes::Rotator:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FRotator>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::String:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*bindValue.Value.GetValue<FString>()), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Widechar:
			{}break;
		case EVariantTypes::Timespan:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FTimespan>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Transform:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FTransform>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::TwoVectors:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FTwoVectors>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::UInt8:
			{
				rc = sqlite3_bind_int64(stmt, bindIndex, bindValue.Value.GetValue<uint8>());
			}break;
		case EVariantTypes::UInt16:
		{
			rc = sqlite3_bind_int64(stmt, bindIndex, bindValue.Value.GetValue<uint16>());
		}break;
		case EVariantTypes::UInt32:
			{
				rc = sqlite3_bind_int64(stmt, bindIndex, bindValue.Value.GetValue<uint32>());
			}break;
		case EVariantTypes::UInt64:
			{
				rc = sqlite3_bind_int64(stmt, bindIndex, bindValue.Value.GetValue<uint64>());
			}break;
		case EVariantTypes::Vector:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FVector>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Vector2d:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FVector2d>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Vector4:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FVector4>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::IntPoint:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FIntPoint>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::IntVector:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FIntVector>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::NetworkGUID:
			{
				rc = sqlite3_bind_text(stmt, bindIndex, TCHAR_TO_UTF8(*(bindValue.Value.GetValue<FNetworkGUID>().ToString())), -1, SQLITE_TRANSIENT);
			}break;
		case EVariantTypes::Custom:
			{
				sqlite3_bind_null(stmt, bindIndex);
			}break;
		default:
			{
				UE_LOG(LogTemp, Warning, TEXT("SQLite: Unsupported bind type at index %d."), bindIndex);
				sqlite3_bind_null(stmt, bindIndex);
			}break;
		}
		if (rc != SQLITE_OK)
		{
			UE_LOG(LogTemp, Error, TEXT("SQLite: Failed to bind value at index %d. Error code: %d"), bindIndex, rc);
			return false;
		}
	}
	
	return true;
}


// void UTIHHsActionSystem::CreateSQLTable()
// {
// 	TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
// 	
// 	FString dbPath =mActionDBPath/ mActionDBName;
//     
// 	// 🔧 폴더 존재 확인 및 생성
// 	FString dbDirectory = FPaths::GetPath(dbPath);
// 	if (!FPaths::DirectoryExists(dbDirectory))
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Creating directory: %s"), *dbDirectory);
// 		if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*dbDirectory))
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *dbDirectory);
// 			return;
// 		}
// 	}
// 	
// 	// 🔧 권한 확인
// 	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*dbDirectory))
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Directory does not exist after creation: %s"), *dbDirectory);
// 		return;
// 	}
// 	
// 	UE_LOG(LogTemp, Warning, TEXT("Attempting to open database: %s"), *dbPath);
// 	
// 	if (sqliteHelper->Open(dbPath))
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Database opened successfully!"));
//         
// 		
// 		bool success = sqliteHelper->ExecuteWrite(
// 			FTIHSQLiteTableBuilder(TEXT("InputActions"))
// 			.IfNotExists()
// 			.WithColumn("action_name",ETIHSQLiteDataType::Text, { FSQLiteConstraints::PrimaryKey() })
// 			.WithColumn("asset_path", ETIHSQLiteDataType::Text, { FSQLiteConstraints::NotNull() })
// 			.WithColumn("is_enabled", ETIHSQLiteDataType::Integer, { FSQLiteConstraints::Default(1) })
// 		);
//
// 		if (success)
// 		{
// 			const FString actionTableName = TEXT("/TIHHousingSystem/resources/enhancedInput/profile1");
// 			// /TIHHousingSystem/resources/enhancedInput/profile1
// 			TArray<TPair<FString,FString>> actionNames =
// 			{
// 				{TEXT("IA_main-select"),		actionTableName /TEXT("common\\IA\\IA_main-select") },
// 				{TEXT("IA_sub-select"),		actionTableName /TEXT("common\\IA\\IA_sub-select") },
// 				{TEXT("IA_cancel"),			actionTableName /TEXT("common\\IA\\IA_cancel") },
// 				{TEXT("IA_cursor-move"),		actionTableName /TEXT("key-mouse\\IA\\IA_cursor-move") },
// 				{TEXT("IA_scroll-wheel"),		actionTableName /TEXT("key-mouse\\IA\\IA_scroll-wheel") }
// 			};
// 			for (auto action : actionNames)
// 			{
// 				success = sqliteHelper->ExecuteWrite(
// 					TIHSQL_INSERT(TEXT("InputActions"))
// 					.Value("action_name", action.Key)
// 					.Value("asset_path", action.Value)
// 					.Value("is_enabled", 1) // 기본적으로 활성화
// 				);
// 				if (!success)
// 				{
// 					UE_LOG(LogTemp, Error, TEXT("Failed to insert action: %s"), *action.Key);
// 				}
// 				else
// 				{
// 					UE_LOG(LogTemp, Log, TEXT("Inserted action: %s"), *action.Key);
// 				}
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Failed to create InputActions table"));
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s"), *dbPath);
//         
// 		// 🔧 추가 디버깅 정보
// 		UE_LOG(LogTemp, Error, TEXT("Directory exists: %s"), 
// 			FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*dbDirectory) ? TEXT("YES") : TEXT("NO"));
// 		UE_LOG(LogTemp, Error, TEXT("Can write to directory: %s"), 
// 			FPlatformFileManager::Get().GetPlatformFile().IsReadOnly(*dbDirectory) ? TEXT("NO") : TEXT("YES"));
// 	}
// 	sqliteHelper->Close();
// }
//
// void UTIHHsActionSystem::LoadInputActionsFromDatabase()
// {
// 	TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
//     
// 	FString dbPath = FPaths::ConvertRelativePathToFull(	mActionDBPath / mActionDBName	);
//     
// 	if (sqliteHelper->Open(dbPath))
// 	{
// 		// 데이터 읽기
// 		bool bSuccess = sqliteHelper->Select(
// 			TIHSQL_SELECT({"action_name", "asset_path", "is_enabled"})
// 			.From("InputActions")
// 			.Where("is_enabled", ETIHSQLiteOperator::EqualTo, 1),
// 			[this](const FSQLiteRowView& Row)
// 			{
// 				FString ActionName = Row.GetColumn<FString>(0);
// 				FString AssetPath = Row.GetColumn<FString>(1);
// 				int32 IsEnabled = Row.GetColumn<int32>(2);
//                 
// 				UE_LOG(LogTemp, Warning, TEXT("Found action: %s -> %s (Enabled: %d)"), 
// 					*ActionName, *AssetPath, IsEnabled);
//                 
// 				// InputAction 로드 시도
// 				UInputAction* Action = LoadObject<UInputAction>(nullptr, *AssetPath);
// 				if (Action)
// 				{
// 					UE_LOG(LogTemp, Warning, TEXT("Successfully loaded InputAction: %s"), *ActionName);
// 					// 바인딩 처리...
// 				}
// 				else
// 				{
// 					UE_LOG(LogTemp, Error, TEXT("Failed to load InputAction: %s from %s"), *ActionName, *AssetPath);
// 				}
// 			}
// 		);
//         
// 		if (!bSuccess)
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Failed to execute SELECT query"));
// 		}
//         
// 		sqliteHelper->Close();
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to open database for reading"));
// 	}
// }
//
// void UTIHHsActionSystem::ReadAndLogDatabaseContents()
// {
// 	TObjectPtr<UTIHHsSQLiteHelper> sqliteHelper = NewObject<UTIHHsSQLiteHelper>();
//     
// 	FString dbPath = FPaths::ConvertRelativePathToFull(mActionDBPath / mActionDBName );
//     
// 	UE_LOG(LogTemp, Warning, TEXT("=== Reading Database Contents ==="));
//     
// 	if (sqliteHelper->Open(dbPath))
// 	{
// 		TArray<FTIHHsActionProfile> actionProfiles;
// 		// 모든 데이터 읽기
// 		bool bSuccess = sqliteHelper->Select(
// 			TIHSQL_SELECT({"action_name", "asset_path", "is_enabled"})
// 			.From("InputActions"),
// 			[&actionProfiles](const FSQLiteRowView& Row)
// 			{
// 				FString ActionName = Row.GetColumn<FString>(0);
// 				FString AssetPath = Row.GetColumn<FString>(1);
// 				int32 IsEnabled = Row.GetColumn<int32>(2);
//
// 				FTIHHsActionProfile actionProfile;
// 				actionProfile.ActionName = ActionName;
// 				actionProfile.AssetPath = AssetPath;
// 				actionProfile.IsEnabled = IsEnabled != 0; // 0이면 비활성화, 1이면 활성화
// 				
// 				if (actionProfile.IsEnabled != 0)
// 				{
// 					actionProfiles.Add(actionProfile);
// 				}
// 				
// 				UE_LOG(LogTemp, Warning, TEXT("Action: %-20s | Path: %-50s | Enabled: %s"), 
// 					*ActionName, 
// 					*AssetPath, 
// 					IsEnabled ? TEXT("✓") : TEXT("✗"));
// 			}
// 		);
// 		
// 		if (bSuccess)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("=== Database read completed ==="));
//
// 			for (const FTIHHsActionProfile& actionProfile :actionProfiles)
// 			{
// 				
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("Failed to read database contents"));
// 		}
//         
// 		sqliteHelper->Close();
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s"), *dbPath);
// 	}  
// }


