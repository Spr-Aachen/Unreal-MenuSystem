// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/* Session functionality handler functions to be called by Menu class */
	//
	void CreateSession(
		int32 NumPublicConnections, // Specify the number of players that can join the game
		FString MatchType // 
	);
	//
	void FindSessions(
		int32 MaxSearchResults // Specify the number of search results
	);
	//
	void JoinSession(
		const FOnlineSessionSearchResult &SessionResult
	);
	//
	void DestroySession();
	//
	void StartSession();

protected:
	/* Callback functions for the session delegates. Notice that each of their input&return params have to match the definition of the corresponding delegate */
	// Callback function which will be called in response to successfully create a game session
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	// Callback function which will be called in response to successfully find game sessions
	void OnFindSessionsComplete(bool bWasSuccessful);
	// Callback function which will be called in response to successfully join the game session
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	// Callback function which will be called in response to successfully destroy the game session
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	// Callback function which will be called in response to successfully start the game session
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	/* Session delegates to bind callback functions */
	// Delegate to bind callback function of session creation
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	// DelegateHandle for CreateSessionCompleteDelegate
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	// Delegate to bind callback function of session search
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	// DelegateHandle for FindSessionsCompleteDelegate
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	// Delegate to bind callback function of session joint
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	// DelegateHandle for JoinSessionCompleteDelegate
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	// Delegate to bind callback function of session destruction
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	// DelegateHandle for DestroySessionCompleteDelegate
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	// Delegate to bind callback function of session start
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	// DelegateHandle for StartSessionCompleteDelegate
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
