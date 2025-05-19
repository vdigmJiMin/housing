// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsBlackboard.h"
#include "TIHHsBlackboard.inl"

#include "TIHBlackBoardSubsystem.h"

//FTIHHsBBTicketHandle

FTIHHsBBRWBase::FTIHHsBBRWBase(const FTIHHsBBTicketHandle& inTicketHandle):
	mValid(true), mBBTicket(nullptr), mTargetBBSession(nullptr),
	mBBTicketElement(nullptr)
{
	static UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	UTIHHsBBSession* bbSession = nullptr;
	TSharedPtr<FTIHHsBBTicket> bbTicket = nullptr;
	
	if (bbSystem != nullptr && bbSystem->IsValidBBTicket(inTicketHandle))
	{
		bbSession = bbSystem->GetBBSessionById(inTicketHandle.GetSessionID());
		bbTicket = bbSystem->GetBBTicketByHandle(inTicketHandle);	
	}
	
	if (bbSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbSystem is nullptr"));
		SetValid(false);
	}
	else if (bbSession == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbSession is nullptr"));
		SetValid(false);
	}
	else if (bbTicket == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbTicket is nullptr"));
		SetValid(false);
	}
	else
	{
		mBBTicket = bbTicket;
		mTargetBBSession = bbSession;
	}
}

FTIHHsBBRWBase::FTIHHsBBRWBase(const FTIHHsBBTicketHandle& inTicketHandle, FTIHHsBBTicketElement& target)
: mValid(true), mBBTicket(nullptr), mTargetBBSession(nullptr),mBBTicketElement(&target)
{
	static UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	UTIHHsBBSession* bbSession = nullptr;
	TSharedPtr<FTIHHsBBTicket> bbTicket = nullptr;
	
	if (bbSystem != nullptr && bbSystem->IsValidBBTicket(inTicketHandle))
	{
		bbSession = bbSystem->GetBBSessionById(inTicketHandle.GetSessionID());
		bbTicket = bbSystem->GetBBTicketByHandle(inTicketHandle);	
	}
	
	if (bbSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbSystem is nullptr"));
		SetValid(false);
	}
	else if (bbSession == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbSession is nullptr"));
		SetValid(false);
	}
	else if (bbTicket == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTIHHsBBRWBase::FTIHHsBBRWBase() - bbTicket is nullptr"));
		SetValid(false);
	}
	else
	{
		mBBTicket = bbTicket;
		mTargetBBSession = bbSession;
	}
}

void FTIHHsBBTicket::MakeExpired(UTIHHsBBSession* inSession)
{
	if (inSession != nullptr)
	{
		inSession->UnRegisterBBTicketHandle(FTIHHsBBTicketHandle(SessionID, TicketID));
	}
	SessionID = -1;
	TicketID = -1;
	ClearReadCount();
}

bool FTIHHsBBTicket::GetTicketElementsForm(TArray<FTIHHsBBTicketElementForm>& outElements) const
{
	if (not TicketElementsView->IsEmpty())
	{
		const TArray<FTIHHsBBTicketElement>& ticketElements = TicketElementsView.Get();
		int32 reserveNum = ticketElements.Num();
		outElements.Reserve(reserveNum);
		
		TSet<int32> checkIndex;
		
		//	namedview
		for (const TPair<FString, FTIHHsBBTicketElementNamedView>& elementNameView :TicketElementsNameMap)
		{
			const FString& elementName =elementNameView.Key;
			const FTIHHsBBTicketElementNamedView& elementView = elementNameView.Value;
			int16 elementType = elementView.GetElementType();
			int16 elementIndex = elementView.GetElementsIndex();

			if (ticketElements.IsValidIndex(elementIndex))
			{
				const FTIHHsBBTicketElement& ticketElement = ticketElements[elementIndex];
				if ( ticketElement.GetDataType() == elementType)
				{
					FTIHHsBBTicketElementForm elementForm;
					elementForm.ElementName = elementName;
					elementForm.ElementType = FTIHHsDefaultTypeTraitHelper::GetTypeFromInt16(elementType);
					elementForm.ElementCount = ticketElement.GetDataCount();
					outElements.Add(elementForm);
					checkIndex.Add(elementIndex);
				}
			}
		}
		//	elementview
		for (int32 viewIdx = 0; viewIdx < reserveNum; ++viewIdx)
		{
			//	중복확인
			if (checkIndex.Contains(viewIdx))
			{
				continue;
			}
			const FTIHHsBBTicketElement& ticketElement = ticketElements[viewIdx];
			if (ticketElement.GetDataType() != static_cast<int16>(ETIHHsBlackBoardDataType::EInvalid))
			{
				FTIHHsBBTicketElementForm elementForm;
				//	elementForm.ElementName = nonamedVariable;
				elementForm.ElementType = FTIHHsDefaultTypeTraitHelper::GetTypeFromInt16(ticketElement.GetDataType());
				elementForm.ElementCount = ticketElement.GetDataCount();
				outElements.Add(elementForm);
			}
		}
		checkIndex.Empty(0);
	}
	if (outElements.IsEmpty())
	{
		return false;
	}
	return true;
}

void UTIHHsBBSession::MakeExpiredTickets()
{
	UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	for (const FTIHHsBBTicketHandle& i : mBlackBoardTicketHandles)
	{
		TSharedPtr<FTIHHsBBTicket> bbTicketValue = bbSystem->GetBBTicketByHandle(i);
		if (bbTicketValue == nullptr)
		{
			continue;
		}
		bbTicketValue->MakeExpired(nullptr);	//	어차피 세션도 날라갈것이므로 미리 날려주고 
		bbSystem->MarkBBTicketForDelete(i);				//	여기에 예약해준다. 예약한 티켓들은 일괄로 나중에 처리를 하든 바로 처리하든할것임
	}
	mBlackBoardTicketHandles.Empty(0);
}

void UTIHHsBBSession::BeginDestroy()
{
	//	최종적으로 없어지는 곳
	UObject::BeginDestroy();
	UE_LOG(LogTemp, Log, TEXT("BeginDestroy %s"), *GetBlackBoardName());
	SetSessionState(ETIHHsBBSessionState::EDestroying);
	MakeExpiredTickets();
}


void UTIHHsBBSession::OnPostCreateSessionDelegate()
{
	UE_LOG(LogTemp, Warning, TEXT("PostCreateSession %s"), *GetBlackBoardName());
}

void UTIHHsBBSession::PreDestroySession()
{
	UE_LOG(LogTemp, Warning, TEXT("PreDestroySession %s"), *GetBlackBoardName());
}

void UTIHHsBBSession::OnTimerExpired()
{
	UTIHHsBlackBoardSubsystem* bbSystem =  UTIHHsBlackBoardSubsystem::GetBBSystem();
	bbSystem->SessionTimerExpired(this);
}

void UTIHHsBBSession::RegisterBBTicketHandle(const FTIHHsBBTicketHandle& inTicketHandle)
{
	mBlackBoardTicketHandles.Add(inTicketHandle);
}

bool UTIHHsBBSession::IsRegistedTicketHandle(const FTIHHsBBTicketHandle& inTicketHandle) const
{
	return mBlackBoardTicketHandles.Contains(inTicketHandle);
}


void UTIHHsBBSession::UseBBTicket(const FTIHHsBBTicketHandle& inTicketHandle)
{
	if (IsRegistedTicketHandle(inTicketHandle))
	{
		UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
		TSharedPtr<FTIHHsBBTicket> inTicket = bbSystem->GetBBTicketByHandle(inTicketHandle);
		if (inTicket != nullptr && not inTicket->IsTicketExpired())
		{
			inTicket->DecrementReadCount();
			if (not inTicket->IsPersist() && inTicket->GetReadCount() <= 0)
			{
				inTicket->MakeExpired(this);	//	세션에서 제거하고
				bbSystem->MarkBBTicketForDelete(inTicketHandle);//	이경우는 제거라 생각했음
					//	티켓은 목록에서 제거되는순간 아예 종료되는거임.
			}
		}
		else
		{
			//	세션에서는 핸들이 등록되어있는데 티켓은 nullptr 이거나 만료된 경우 그냥 제거
			mBlackBoardTicketHandles.Remove(inTicketHandle);
		}
	}
	if (mBlackBoardTicketHandles.IsEmpty() && mSessionData.SessionType == ETIHHsBBSessionCloseType::EReferenceCountBase)
	{
		UTIHHsBlackBoardSubsystem::GetBBSystem()->MarkBBSessionForDelete(this);
	}
	
}
