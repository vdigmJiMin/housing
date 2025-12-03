// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "sqlite/sqlite3.h"

#include "TIHHsSQLiteHelper.generated.h"


//	enum classes
enum class ETIHSQLiteDataType : uint8;
enum class ETIHSQLiteConstraint : uint8;
enum class ETIHSQLiteOperator : uint8;
enum class ETIHSQLiteOrderBy : uint8;
enum class ETIHSQLiteJoinType : uint8;

//	structs
struct FTIHSQLiteBindValue;
struct FTIHSQLiteTableConstraint;

//	builders
class FTIHSQLiteTableBuilder ;
class FTIHSQLiteSelectBuilder;
class FTIHSQLiteInsertBuilder;
class FTIHSQLiteUpdateBuilder;

//	helepers
class FTIHHsSQLiteQueryEnumHelper;
class UTIHHsSQLiteHelper;

UENUM(BlueprintType)
enum class ETIHSQLiteDataType : uint8
{
	Integer,
	Real,
	Text,
	Blob
};
UENUM(BlueprintType)
enum class ETIHSQLiteConstraint : uint8
{
	PrimaryKey,
	AutoIncrement,
	NotNull,
	Unique,
	Default,
};
UENUM(BlueprintType)
enum class ETIHSQLiteOperator : uint8
{
	EqualTo,            // =
	NotEqualTo,         // !=
	GreaterThan,        // >
	LessThan,           // <
	GreaterThanOrEqual, // >=
	LessThanOrEqual,    // <=
	Like                // LIKE
};
UENUM(BlueprintType)
enum class ETIHSQLiteOrderBy : uint8
{
	Ascending,  // ASC
	Descending  // DESC
};

// JOIN 타입
UENUM(BlueprintType)
enum class ETIHSQLiteJoinType : uint8
{
	Inner,
	Left
};

struct FSQLiteColumnConstraint
{
	// 가상 소멸자. 상속을 사용하므로 필수입니다.
	virtual ~FSQLiteColumnConstraint() = default;
	// 이 제약조건을 SQL 문자열로 변환하는 가상 함수
	virtual FString ToSql() const = 0;
};
// --- 값 없는 단순 제약조건들 ---
struct FPrimaryKeyConstraint : public FSQLiteColumnConstraint
{
	virtual FString ToSql() const override { return TEXT("PRIMARY KEY"); }
};

struct FAutoIncrementConstraint : public FSQLiteColumnConstraint
{
	virtual FString ToSql() const override { return TEXT("AUTOINCREMENT"); }
};

struct FNotNullConstraint : public FSQLiteColumnConstraint
{
	virtual FString ToSql() const override { return TEXT("NOT NULL"); }
};

struct FUniqueConstraint : public FSQLiteColumnConstraint
{
	virtual FString ToSql() const override { return TEXT("UNIQUE"); }
};

// --- 값이 필요한 DEFAULT 제약조건 ---
struct FDefaultConstraint : public FSQLiteColumnConstraint
{
	FVariant DefaultValue;

	// 생성자에서 FVariant를 사용하여 모든 타입의 기본값을 받습니다.
	template<typename T>
	FDefaultConstraint(T InValue) : DefaultValue(InValue) {}

	virtual FString ToSql() const override
	{
		FString ValueString;
		// 값의 타입에 따라 문자열 포맷을 다르게 합니다. (문자열은 ''로 감싸기)
		switch (DefaultValue.GetType())
		{
		case EVariantTypes::String:
		case EVariantTypes::Name:
			ValueString = FString::Printf(TEXT("'%s'"), *DefaultValue.GetValue<FString>());
			break;
			// int, double 등 다른 타입들은 그냥 문자열로 변환
		default:
			// FVariant에 AsString이 없으므로, 각 타입별로 처리 필요
				if(DefaultValue.GetType() == EVariantTypes::Int32) ValueString = FString::FromInt(DefaultValue.GetValue<int32>());
				else if(DefaultValue.GetType() == EVariantTypes::Double) ValueString = FString::SanitizeFloat(DefaultValue.GetValue<double>());
			// ... 기타 타입 처리 ...
			break;
		}
		return FString::Printf(TEXT("DEFAULT %s"), *ValueString);
	}
};

struct FSQLiteConstraints
{
	static TSharedRef<FSQLiteColumnConstraint> PrimaryKey() { return MakeShared<FPrimaryKeyConstraint>(); }
	static TSharedRef<FSQLiteColumnConstraint> AutoIncrement() { return MakeShared<FAutoIncrementConstraint>(); }
	static TSharedRef<FSQLiteColumnConstraint> NotNull() { return MakeShared<FNotNullConstraint>(); }
	static TSharedRef<FSQLiteColumnConstraint> Unique() { return MakeShared<FUniqueConstraint>(); }

	template<typename T>
	static TSharedRef<FSQLiteColumnConstraint> Default(T InValue)
	{
		return MakeShared<FDefaultConstraint>(InValue);
	}
};

class FTIHHsSQLiteQueryEnumHelper
{
public:
	static FString OperatorToString(ETIHSQLiteOperator Op)
	{
		switch (Op)
		{
			case ETIHSQLiteOperator::EqualTo:            return TEXT("=");
			case ETIHSQLiteOperator::NotEqualTo:         return TEXT("!=");
			case ETIHSQLiteOperator::GreaterThan:        return TEXT(">");
			case ETIHSQLiteOperator::LessThan:           return TEXT("<");
			case ETIHSQLiteOperator::GreaterThanOrEqual: return TEXT(">=");
			case ETIHSQLiteOperator::LessThanOrEqual:    return TEXT("<=");
			case ETIHSQLiteOperator::Like:               return TEXT("LIKE");
		}
		return TEXT("");
	}

	static FString OrderByToString(ETIHSQLiteOrderBy Direction)
	{
		return Direction == ETIHSQLiteOrderBy::Ascending ? TEXT("ASC") : TEXT("DESC");
	}

	static FString JoinTypeToString(ETIHSQLiteJoinType JoinType)
	{
		return JoinType == ETIHSQLiteJoinType::Inner ? TEXT("INNER") : TEXT("LEFT");
	}
	static FString ConstraintToString(ETIHSQLiteConstraint Constraint)
	{
		switch (Constraint)
		{
		case ETIHSQLiteConstraint::PrimaryKey:    return TEXT("PRIMARY KEY");
		case ETIHSQLiteConstraint::AutoIncrement: return TEXT("AUTOINCREMENT");
		case ETIHSQLiteConstraint::NotNull:       return TEXT("NOT NULL");
		case ETIHSQLiteConstraint::Unique:        return TEXT("UNIQUE");
		case ETIHSQLiteConstraint::Default:
			// Default 제약조건은 별도의 값이 필요하므로 빈 문자열 반환
			// 실제 사용 시에는 이 부분을 수정하여 기본값을 지정할 수 있습니다.
			return TEXT("DEFAULT");
		default: ;
		}
		return TEXT("");
	}
	// Enum을 실제 SQL 문자열로 변환하는 헬퍼 함수
	static FString DataTypeToString(ETIHSQLiteDataType DataType)
	{
		switch (DataType)
		{
		case ETIHSQLiteDataType::Integer: return TEXT("INTEGER");
		case ETIHSQLiteDataType::Real:    return TEXT("REAL");
		case ETIHSQLiteDataType::Text:    return TEXT("TEXT");
		case ETIHSQLiteDataType::Blob:    return TEXT("BLOB");
		}
		return TEXT("");
	}
};


struct FTIHSQLiteBindValue
{
	FVariant Value;
	FTIHSQLiteBindValue(int32 InValue) : Value(InValue) {}
	FTIHSQLiteBindValue(int64 InValue) : Value(InValue) {}
	FTIHSQLiteBindValue(double InValue) : Value(InValue) {}
	FTIHSQLiteBindValue(const FString& InValue) : Value(InValue) {}
	FTIHSQLiteBindValue(const TArray<uint8>& InValue) : Value(InValue) {}
	EVariantTypes GetType() const { return Value.GetType(); }
};


struct FTIHSQLiteTableConstraint
{
	FString ConstraintString;

	static FTIHSQLiteTableConstraint ForeignKey(const FString& InColumnName)
	{
		return { FString::Printf(TEXT("FOREIGN KEY (%s)"), *InColumnName) };
	}

	FTIHSQLiteTableConstraint& References(const FString& TargetTable, const FString& TargetColumn)
	{
		ConstraintString.Append(FString::Printf(TEXT(" REFERENCES %s (%s)"), *TargetTable, *TargetColumn));
		return *this;
	}
};



class FTIHSQLiteTableBuilder
{
public:
    // 생성자: 테이블 이름과 함께 시작
    FTIHSQLiteTableBuilder(const FString& tableName):mTableName(tableName), mHasColumns(false), mUseIfNotExists(false)
    {
	   
    }
	FTIHSQLiteTableBuilder& IfNotExists()
    {
    	mUseIfNotExists = true;
    	return *this; // 메서드 체이닝을 위해 *this 반환
    }

  
	FTIHSQLiteTableBuilder& WithColumn(
	const FString& ColumnName, 
	ETIHSQLiteDataType DataType, 
	const TArray<TSharedRef<FSQLiteColumnConstraint>>& Constraints = {})
    {
    	FString ColumnString = FString::Printf(TEXT("%s %s"), *ColumnName, *FTIHHsSQLiteQueryEnumHelper::DataTypeToString(DataType));

    	// 배열을 순회하며 각 제약조건 객체의 ToSql() 함수를 호출합니다.
    	for (const TSharedRef<FSQLiteColumnConstraint>& Constraint : Constraints)
    	{
    		ColumnString.Append(FString::Printf(TEXT(" %s"), *Constraint->ToSql()));
    	}

    	ColumnDefinitions.Add(ColumnString);
    	return *this;
    }

    // 테이블 제약조건 추가
    FTIHSQLiteTableBuilder& WithTableConstraint(const FString& ColumnName, ETIHSQLiteDataType DataType, const TArray<ETIHSQLiteConstraint>& Constraints)
    {
    	FString ColumnString = FString::Printf(TEXT("%s %s"), *ColumnName, *FTIHHsSQLiteQueryEnumHelper::DataTypeToString(DataType));
    	for (const ETIHSQLiteConstraint& Constraint : Constraints)
    	{
    		ColumnString.Append(FString::Printf(TEXT(" %s"), *FTIHHsSQLiteQueryEnumHelper::ConstraintToString(Constraint)));
    	}
    	ColumnDefinitions.Add(ColumnString);
    	return *this;
    }

    // 쿼리 문자열 최종 완성
    FString Build() const
    {
    	FString Query = TEXT("CREATE TABLE ");

    	if (mUseIfNotExists)
    	{
    		Query.Append(TEXT("IF NOT EXISTS "));
    	}
    	Query.Append(FString::Printf(TEXT("%s ("), *mTableName));

    	// 1. 컬럼 정의와 테이블 제약조건 정의를 하나의 배열로 합칩니다.
    	TArray<FString> AllDefinitions;
    	AllDefinitions.Append(ColumnDefinitions);
    	AllDefinitions.Append(TableConstraintDefinitions);

    	// 2. 합쳐진 배열의 모든 항목을 ", "로 연결합니다.
    	Query.Append(FString::Join(AllDefinitions, TEXT(", ")));

    	Query.Append(TEXT(");"));
    
    	return Query;
    }

private:
    //FString mQueryString;
	FString mTableName;
    bool mHasColumns ;
	bool mUseIfNotExists ; // IF NOT EXISTS 옵션 사용 여부

	TArray<FString> ColumnDefinitions;
	TArray<FString> TableConstraintDefinitions;

};
class FTIHSQLiteSelectBuilder
{
public:
    FTIHSQLiteSelectBuilder(const TArray<FString>& InColumns)
    {
        // TArray<FString>를 "Col1, Col2, ..." 형태의 문자열로 변환
        SelectClause = FString::Join(InColumns, TEXT(", "));
    }

    FTIHSQLiteSelectBuilder& From(const FString& TableName)
    {
        FromClause = TableName;
        return *this;
    }

    FTIHSQLiteSelectBuilder& Join(ETIHSQLiteJoinType JoinType, const FString& TargetTable, const FString& OnCondition)
    {
        FString JoinString = FString::Printf(TEXT(" %s JOIN %s ON %s"),
        	
            *FTIHHsSQLiteQueryEnumHelper::JoinTypeToString(JoinType), *TargetTable, *OnCondition);
        JoinClauses.Add(JoinString);
        return *this;
    }

    // 다양한 타입을 받을 수 있도록 템플릿으로 WHERE 함수 구현
    template<typename T>
    FTIHSQLiteSelectBuilder& Where(const FString& Column, ETIHSQLiteOperator Op, T InValue)
    {
        FString WhereString = FString::Printf(TEXT("%s %s ?"), *Column, *FTIHHsSQLiteQueryEnumHelper::OperatorToString(Op));
        WhereClauses.Add(WhereString);
        BindValues.Add(FTIHSQLiteBindValue(InValue));
        return *this;
    }

    FTIHSQLiteSelectBuilder& OrderBy(const FString& Column, ETIHSQLiteOrderBy Direction = ETIHSQLiteOrderBy::Ascending)
    {
        OrderByClause = FString::Printf(TEXT(" ORDER BY %s %s"), *Column, *FTIHHsSQLiteQueryEnumHelper::OrderByToString(Direction));
        return *this;
    }

    FTIHSQLiteSelectBuilder& Limit(int32 Count)
    {
        LimitClause = FString::Printf(TEXT(" LIMIT %d"), Count);
        return *this;
    }

    // 최종적으로 쿼리 문자열과 바인딩할 값 목록을 생성
    void Build(FString& OutQuery, TArray<FTIHSQLiteBindValue>& OutBindValues)
    {
        OutQuery = FString::Printf(TEXT("SELECT %s FROM %s"), *SelectClause, *FromClause);

        if (JoinClauses.Num() > 0)
        {
            OutQuery.Append(FString::Join(JoinClauses, TEXT("")));
        }
        if (WhereClauses.Num() > 0)
        {
            OutQuery.Append(TEXT(" WHERE "));
            OutQuery.Append(FString::Join(WhereClauses, TEXT(" AND ")));
        }
        if (!OrderByClause.IsEmpty())
        {
            OutQuery.Append(OrderByClause);
        }
        if (!LimitClause.IsEmpty())
        {
            OutQuery.Append(LimitClause);
        }
        OutQuery.Append(TEXT(";"));
        
        OutBindValues = BindValues;
    }

private:
    FString SelectClause;
    FString FromClause;
    TArray<FString> JoinClauses;
    TArray<FString> WhereClauses;
    FString OrderByClause;
    FString LimitClause;
    TArray<FTIHSQLiteBindValue> BindValues;
};
class FTIHSQLiteInsertBuilder
{
public:
	FTIHSQLiteInsertBuilder(const FString& TableName)
		: mInsertClause(FString::Printf(TEXT("INSERT INTO %s"), *TableName))
	{
	}

	template<typename T>
	FTIHSQLiteInsertBuilder& Value(const FString& ColumnName, T InValue)
	{
		mColumnNames.Add(ColumnName);
		mBindValues.Add(FTIHSQLiteBindValue(InValue));
		return *this;
	}
	
	FTIHSQLiteInsertBuilder& Value(const FString& columnName, const TArray<uint8>& inValue)
	{
		mColumnNames.Add(columnName);
		// 위에서 추가한 생성자 덕분에 이 코드가 정상적으로 동작합니다.
		mBindValues.Add(FTIHSQLiteBindValue(inValue));
		return *this;
	}
	FTIHSQLiteInsertBuilder& Value(const FString& columnName, const FVector& inValue)
	{
		mColumnNames.Add(columnName);
		// FVector를 FString으로 변환하여 바인딩 리스트에 추가합니다.
		mBindValues.Add(FTIHSQLiteBindValue(inValue.ToString()));
		return *this;
	}

	void Build(FString& OutQuery, TArray<FTIHSQLiteBindValue>& OutBindValues)
	{
		if (mColumnNames.Num() == 0) return;

		// (Column1, Column2, ...) 부분 생성
		FString columnPart = FString::Printf(TEXT("(%s)"), *FString::Join(mColumnNames, TEXT(", ")));

		// (?, ?, ...) 부분 생성
		TArray<FString> placeholders;
		placeholders.Init(TEXT("?"), mColumnNames.Num());
		FString valuesPart = FString::Printf(TEXT("(%s)"), *FString::Join(placeholders, TEXT(", ")));

		OutQuery = FString::Printf(TEXT("%s %s VALUES %s;"), *mInsertClause, *columnPart, *valuesPart);
		OutBindValues = mBindValues;
	}

private:
	FString mInsertClause;
	TArray<FString> mColumnNames;
	TArray<FTIHSQLiteBindValue> mBindValues;
};
class FTIHSQLiteUpdateBuilder
{
public:
	FTIHSQLiteUpdateBuilder(const FString& TableName)
		: UpdateClause(FString::Printf(TEXT("UPDATE %s"), *TableName))
	{
	}

	template<typename T>
	FTIHSQLiteUpdateBuilder& Set(const FString& ColumnName, T InValue)
	{
		SetClauses.Add(FString::Printf(TEXT("%s = ?"), *ColumnName));
		SetBindValues.Add(FSQLiteBindValue(InValue));
		return *this;
	}

	template<typename T>
	FTIHSQLiteUpdateBuilder& Where(const FString& Column, ETIHSQLiteOperator Op, T InValue)
	{
		WhereClauses.Add(FString::Printf(TEXT("%s %s ?"), *Column, *FTIHHsSQLiteQueryEnumHelper::OperatorToString(Op)));
		WhereBindValues.Add(FSQLiteBindValue(InValue));
		return *this;
	}

	void Build(FString& OutQuery, TArray<FTIHSQLiteBindValue>& OutBindValues)
	{
		if (SetClauses.Num() == 0) return;

		OutQuery = FString::Printf(TEXT("%s SET %s"), *UpdateClause, *FString::Join(SetClauses, TEXT(", ")));
        
		if (WhereClauses.Num() > 0)
		{
			OutQuery.Append(FString::Printf(TEXT(" WHERE %s"), *FString::Join(WhereClauses, TEXT(" AND "))));
		}
		OutQuery.Append(TEXT(";"));

		// 최종 바인딩 값 배열 생성 (SET 값들 먼저, 그 다음에 WHERE 값들)
		OutBindValues.Append(SetBindValues);
		OutBindValues.Append(WhereBindValues);
	}

private:
	FString UpdateClause;
	TArray<FString> SetClauses;
	TArray<FTIHSQLiteBindValue> SetBindValues;
	TArray<FString> WhereClauses;
	TArray<FTIHSQLiteBindValue> WhereBindValues;
	
};

class FSQLiteQueryBuilder
{
public:
	static FTIHSQLiteTableBuilder CreateTable(const FString& TableName) { return FTIHSQLiteTableBuilder(TableName); }
	static FTIHSQLiteSelectBuilder Select(const TArray<FString>& Columns) { return FTIHSQLiteSelectBuilder(Columns); }
	static FTIHSQLiteInsertBuilder InsertInto(const FString& TableName) { return FTIHSQLiteInsertBuilder(TableName); }
	static FTIHSQLiteUpdateBuilder Update(const FString& TableName) { return FTIHSQLiteUpdateBuilder(TableName); }
};


USTRUCT(BlueprintType)
struct FTIHHsSQLitePtrWrapper
{
	GENERATED_BODY()
	sqlite3* SQLitePtr;
	
	FTIHHsSQLitePtrWrapper()
		: SQLitePtr(nullptr)
	{
	}
	FTIHHsSQLitePtrWrapper(sqlite3* stmt)
		: SQLitePtr(stmt)
	{
	}
	FTIHHsSQLitePtrWrapper(const FTIHHsSQLitePtrWrapper& other)
		: SQLitePtr(other.SQLitePtr)
	{
	}
	FTIHHsSQLitePtrWrapper& operator=(const FTIHHsSQLitePtrWrapper& other)
	{
		if (this != &other)
		{
			SQLitePtr = other.SQLitePtr;
		}
		return *this;
	}
	FORCEINLINE bool IsValid() const
	{
		return SQLitePtr != nullptr;
	}
	FORCEINLINE sqlite3* Get() const
	{
		return SQLitePtr;
	}
	FORCEINLINE sqlite3* Get()
	{
		return SQLitePtr;
	}
	FORCEINLINE void Close()
	{
		if (SQLitePtr!= nullptr)
		{
			sqlite3_close(SQLitePtr);
			SQLitePtr = nullptr;
		}
	}
	FORCEINLINE void Reset()
	{
		Close();
	}
	FORCEINLINE bool operator==(const FTIHHsSQLitePtrWrapper& other) const
	{
		return SQLitePtr == other.SQLitePtr;
	}
	FORCEINLINE bool operator!=(const FTIHHsSQLitePtrWrapper& other) const
	{
		return !(*this == other);
	}
	sqlite3* operator*() const
	{
		return SQLitePtr;
	}
};
USTRUCT(BlueprintType)
struct FTIHHsSQLiteStmtWrapper
{
	GENERATED_BODY()

	sqlite3_stmt* SQLiteStmt;

	FTIHHsSQLiteStmtWrapper()
		: SQLiteStmt(nullptr)
	{
	}
	FTIHHsSQLiteStmtWrapper(sqlite3_stmt* stmt)
		: SQLiteStmt(stmt)
	{
	}
	FTIHHsSQLiteStmtWrapper(const FTIHHsSQLiteStmtWrapper& other)
		: SQLiteStmt(other.SQLiteStmt)
	{
	}

	sqlite3_stmt* operator*() const
	{
		return SQLiteStmt;
	}
};

class FSQLiteRowView
{
public:
	FSQLiteRowView(sqlite3_stmt* InStmt) : mStmt(InStmt) {}

	template<typename T>
	T GetColumn(int32 ColumnIndex) const; // .cpp에서 템플릿 특수화로 구현

private:
	sqlite3_stmt* mStmt;
};

template<>
inline FString FSQLiteRowView::GetColumn<FString>(int32 ColumnIndex) const {
	const char* Text = reinterpret_cast<const char*>(sqlite3_column_text(mStmt, ColumnIndex));
	return Text ? UTF8_TO_TCHAR(Text) : FString();
}
template<>
inline int32 FSQLiteRowView::GetColumn<int32>(int32 columnIndex) const {
	return sqlite3_column_int(mStmt, columnIndex);
}
template<>
inline int64 FSQLiteRowView::GetColumn<int64>(int32 columnIndex) const {
	return sqlite3_column_int64(mStmt, columnIndex);
}
template<>
inline float FSQLiteRowView::GetColumn<float>(int32 columnIndex) const {
	return static_cast<float>(sqlite3_column_double(mStmt, columnIndex));
}
template<>
inline double FSQLiteRowView::GetColumn<double>(int32 columnIndex) const {
	return sqlite3_column_double(mStmt, columnIndex);
}
template<>
inline bool FSQLiteRowView::GetColumn<bool>(int32 columnIndex) const {
	return sqlite3_column_int(mStmt, columnIndex) != 0;
}
template<>
inline FVector FSQLiteRowView::GetColumn<FVector>(int32 columnIndex) const
{
	// 1. 데이터베이스에서 TEXT 값을 FString으로 먼저 읽어옵니다.
	const FString vectorString = GetColumn<FString>(columnIndex);
    
	FVector resultVector = FVector::ZeroVector;
	if (!vectorString.IsEmpty())
	{
		// 2. FVector의 내장 함수인 InitFromString을 사용하여 문자열을 FVector 값으로 파싱합니다.
		resultVector.InitFromString(vectorString);
	}
    
	return resultVector;
}

template<>
inline TArray<uint8> FSQLiteRowView::GetColumn<TArray<uint8>>(int32 columnIndex) const
{
	TArray<uint8> blobData;
	// BLOB 데이터의 포인터와 크기를 가져옵니다.
	const void* BlobPtr = sqlite3_column_blob(mStmt, columnIndex);
	const int32 BlobSize = sqlite3_column_bytes(mStmt, columnIndex);

	if (BlobPtr && BlobSize > 0)
	{
		// 가져온 데이터 크기만큼 배열을 초기화하고 내용을 복사합니다.
		blobData.SetNumUninitialized(BlobSize);
		FMemory::Memcpy(blobData.GetData(), BlobPtr, BlobSize);
	}
	return blobData;
}

UCLASS()
class TIHHOUSINGCORE_API UTIHHsSQLiteHelper : public UObject
{
	GENERATED_BODY()

public:
	UTIHHsSQLiteHelper();
	virtual ~UTIHHsSQLiteHelper() override;
	
	UFUNCTION()
	bool Open(const FString& databasePath);
	UFUNCTION()
	bool Close();
	UFUNCTION()
	bool IsOpen()const {return mIsOpen; }
	UFUNCTION()
	FTIHHsSQLitePtrWrapper GetDataBase()const {return mDataBaseWrapper;}

	virtual void BeginDestroy() override;

#define TIHSQL_CREATETABLE FSQLiteQueryBuilder::CreateTable
#define TIHSQL_INSERT FSQLiteQueryBuilder::InsertInto
#define TIHSQL_SELECT FSQLiteQueryBuilder::Select
#define TIHSQL_UPDATE FSQLiteQueryBuilder::Update
	
	bool ExecuteWrite(const FTIHSQLiteTableBuilder& Builder);
	bool ExecuteWrite(const FTIHSQLiteInsertBuilder& Builder);
	bool ExecuteWrite(const FTIHSQLiteUpdateBuilder& Builder);
	
	bool ExecuteRawWrite(const FString& query);
	bool ExecuteRawRead(const FString& query, TFunction<void(const FSQLiteRowView& Row)> onRow);
	
	bool Select(const FTIHSQLiteSelectBuilder& Builder, TFunction<void(const FSQLiteRowView& Row)> OnRow);
	
	UFUNCTION()
	static void ExampleCode0();
	//	use builder

	
	UFUNCTION(Exec,CallInEditor,BlueprintCallable, Category = "TIHHsSQLiteHelper")
	static void ExampleCode1();
	UFUNCTION(Exec,CallInEditor,BlueprintCallable, Category = "TIHHsSQLiteHelper")
	static void ExampleCode2_Insert();
	UFUNCTION(Exec,CallInEditor,BlueprintCallable, Category = "TIHHsSQLiteHelper")
	static void ExampleCode3_Select();	
private:
	bool PrepareStatement(const FString& query, sqlite3_stmt*& OutStmt) const;
	void FinalizeStatement(sqlite3_stmt* Stmt);
	bool BindValues(sqlite3_stmt* Stmt, const TArray<FTIHSQLiteBindValue>& BindValues);
	
	UPROPERTY()
	bool mIsOpen;
	
	UPROPERTY()
	FTIHHsSQLitePtrWrapper mDataBaseWrapper;

	UFUNCTION()
	bool Execute_deprecated(const FString& query);	//	제거예정
	UFUNCTION()
	bool PrepareStatement_deprecated(const FString& query, FTIHHsSQLiteStmtWrapper& statement);	//	제거예정
	UFUNCTION()
	void FinalizeStatement_deprecated(FTIHHsSQLiteStmtWrapper& statement);
};
