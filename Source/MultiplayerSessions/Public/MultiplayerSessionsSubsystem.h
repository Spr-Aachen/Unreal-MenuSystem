// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

// Header files with '.generated' should be put in the end
#include "MultiplayerSessionsSubsystem.generated.h"


/*
Declare our own custom multicast delegates (which means once it's broadcast, multiple classes can bind their ufunctions to it)
Notice the difference between DYNAMIC_MULTICAST_DELEGATE macro and MULTICAST_DELEGATE macro
*/
// Declare a dynamic multicast delegate that is capable of binding a ufunction that takes one parameter
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( // No worry if having red squiggles, those will go away after compiling (debug anyway)
	FMultiplayerOnCreateSessionComplete, // Decide on a name for the delegate
	bool, // Indicate the input parameter type of the function that can be bound to the delegate
	bWasSuccessful // Provide a name for the input parameter
);
// Declare a multicast delegate that is capable of binding a function that takes two parameters
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FMultiplayerOnFindSessionsComplete, // Decide on a name for the delegate
	const TArray<FOnlineSessionSearchResult>& SessionResults, // Indicate the first input parameter of the function that can be bound to the delegate
	bool bWasSuccessful // Indicate the second input parameter of the function that can be bound to the delegate
);
// Declare a multicast delegate that is capable of binding a function that takes one parameter
DECLARE_MULTICAST_DELEGATE_OneParam(
	FMultiplayerOnJoinSessionsComplete, // Decide on a name for the delegate
	EOnJoinSessionCompleteResult::Type Result // Indicate the input parameter of the function that can be bound to the delegate
);
// Declare a dynamic multicast delegate that is capable of binding a ufunction that takes one parameter
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( // No worry if having red squiggles, those will go away after compiling (debug anyway)
	FMultiplayerOnDestroySessionComplete, // Decide on a name for the delegate
	bool, // Indicate the input parameter type of the function that can be bound to the delegate
	bWasSuccessful // Provide a name for the input parameter
);
// Declare a dynamic multicast delegate that is capable of binding a ufunction that takes one parameter
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( // No worry if having red squiggles, those will go away after compiling (debug anyway)
	FMultiplayerOnStartSessionComplete, // Decide on a name for the delegate
	bool, // Indicate the input parameter type of the function that can be bound to the delegate
	bWasSuccessful // Provide a name for the input parameter
);


UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

private:
	// Smart pointer to hold the online session interface
	IOnlineSessionPtr SessionInterface;

	// TSharedPtr to store the session settings that we last created
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	// TSharedPtr to store the session search that we last used
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

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

public:
	/* Custom multicast delegates to bind callback functions */
	// Dynamic multicast delegate to bind callback ufunctions of session creation result
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	// Multicast delegate to bind callback functions of session search result
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	// Multicast delegate to bind callback functions of session joint result
	FMultiplayerOnJoinSessionsComplete MultiplayerOnJoinSessionsComplete;
	// Dynamic multicast delegate to bind callback ufunctions of session destruction result
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	// Dynamic multicast delegate to bind callback ufunctions of session start result
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

private:
	/* Store these for the last session we failed to created */
	bool bCreateSessionOnDestroy{false};
	int32 LastNumPublicConnections;
	FString LastMatchType;

public:
	/* Session functionality handler functions */
	// Function to create game session
	void CreateSession(
		int32 NumPublicConnections, // Specify the number of players that can join the game
		FString MatchType // Specify the match type
	);
	// Function to find game sessions
	void FindSessions(
		int32 MaxSearchResults // Specify the number of search results
	);
	// Function to join game session
	void JoinSession(
		const FOnlineSessionSearchResult &SessionResult
	);
	// Function to destroy game session
	void DestroySession();
	// Function to start game session
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
};
