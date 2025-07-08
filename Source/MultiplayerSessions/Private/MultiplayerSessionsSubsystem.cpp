// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
    /* Construct delegates that took in the address of callback function */
    CreateSessionCompleteDelegate(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete)
    ),
    FindSessionsCompleteDelegate(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete)
    ),
    JoinSessionCompleteDelegate(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete)
    ),
    DestroySessionCompleteDelegate(
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete)
    ),
    StartSessionCompleteDelegate(
        FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnStartSessionComplete)
    )

{
    // Get session interface
    IOnlineSubsystem *OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
    }
}


void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
}


void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
}


void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult &SessionResult)
{
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}


void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}


void UMultiplayerSessionsSubsystem::StartSession()
{

}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
