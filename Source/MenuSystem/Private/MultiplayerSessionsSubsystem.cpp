// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
    /*
    Construct delegates that took in the address of callback function
    */
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
    ){
    /*
    Get session interface
    */
    IOnlineSubsystem *OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem){
        SessionInterface = OnlineSubsystem->GetSessionInterface();
    }
}


void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType){
    // Check if SessionInterface is not valid
    if (!SessionInterface.IsValid()){ // The way to check if TSharedPtr is valid is by using the 'IsValid' function
        return;
    }

    /*
    Check and destroy the existing session
    */
    // Get the existing session pointer
    auto ExistingSession = SessionInterface->GetNamedSession(
        NAME_GameSession // If we always use this NAME_GameSession, then we'll always be checking to see if a session with this name exists
    );
	// Check to see if the existing session pointer is not null
	if (ExistingSession != nullptr)
	{
        /* Destroy the existing session */
        // SessionInterface->DestroySession(
        //     NAME_GameSession // Same as above
        // ); // We should be handling this call in DestroySession rather than accessing the SessionInterface directly
        bCreateSessionOnDestroy = true;
        LastNumPublicConnections = NumPublicConnections;
        LastMatchType = MatchType;
        DestroySession();
    }

	/*
    Create a new session
    */
    // Add CreateSessionCompleteDelegate to SessionInterface's delegate list and store it in a FDelegateHandle
    CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate); // Once a session is created, then the fallback function which bound to this delegate will be called
    // Initialize LastSessionSettings TSharedPtr to class FOnlineSessionSettings
    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    // Configure session settings
    LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "Null" ? true : false; // Using ternary operator by checking SubsystemName to decide whether to connect over the internet
    LastSessionSettings->NumPublicConnections = NumPublicConnections; // Determine how many players can connect to the game
	LastSessionSettings->bAllowJoinInProgress = true; // Allow players to join when session is running
    LastSessionSettings->bAllowJoinViaPresence = true; // Allow steam to search for sessions going on players' regions
    LastSessionSettings->bShouldAdvertise = true; // Allow steam to advertise sessions
	LastSessionSettings->bUsesPresence = true; // Allow players to find sessions going on their regions
    //LastSessionSettings->bUseLobbiesIfAvailable = true; // Fix sessions finding issue
    LastSessionSettings->Set( // Specify a match type so we can check that type once we've found the session
        FName("MatchType"), // FName key to define a match type
        MatchType, // FString value to define the match type
        EOnlineDataAdvertisementType::ViaOnlineServiceAndPing // Session will be advertised via the online service and ping
    );
    LastSessionSettings->BuildUniqueId = 1; // Allow multiple users to launch their own build and host
    // Get the world's first local player
    const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    bool IsCreationSuccessful = SessionInterface->CreateSession(
        *LocalPlayer->GetPreferredUniqueNetId(),
        NAME_GameSession,
        *LastSessionSettings
    );
    // If session creation is failed
    if (!IsCreationSuccessful){
        // Remove CreateSessionCompleteDelegate from the list
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        // Broadcast custom multicast delegate
        MultiplayerOnCreateSessionComplete.Broadcast(
            false
        );
    }
}


void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful){
    if (SessionInterface){
        // Remove CreateSessionCompleteDelegate from the list
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
    }
    // Broadcast custom multicast delegate
    MultiplayerOnCreateSessionComplete.Broadcast(
        bWasSuccessful
    );
}


void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults){
    // Check if SessionInterface is not valid
    if (!SessionInterface.IsValid()) // The way to check if TSharedPtr is valid is by using the 'IsValid' function
	{
		return;
	}

    /*
    Find game sessions
    */
    // Add FindSessionsCompleteDelegate to SessionInterface's delegate list and store it in a FDelegateHandle
    FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate); // Once all sessions are found, then the fallback function which bound to this delegate will be called
    // Initialize LastSessionSearch TSharedPtr to class FOnlineSessionSearch
    LastSessionSearch = MakeShareable(new FOnlineSessionSearch);
	// Configure search settings
    LastSessionSearch->MaxSearchResults = MaxSearchResults;
    LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "Null" ? true : false; // Using ternary operator by checking SubsystemName to decide whether to connect over the internet
    LastSessionSearch->QuerySettings.Set( // Make sure any session we find is using presence
		SEARCH_PRESENCE, // Macro
		true,
		EOnlineComparisonOp::Equals
	);
    // Get the world's first local player
    const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool IsSearchSuccessful = SessionInterface->FindSessions(
		*LocalPlayer->GetPreferredUniqueNetId(),
		LastSessionSearch.ToSharedRef()
	);
    // If sessions search is failed
    if (!IsSearchSuccessful){
        // Remove FindSessionsCompleteDelegate from the list
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
        // Broadcast custom multicast delegate
        MultiplayerOnFindSessionsComplete.Broadcast(
            TArray<FOnlineSessionSearchResult>(), // Empty array
            false
        );
    }
}


void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful){
    if (SessionInterface){
        // Remove FindSessionsCompleteDelegate from the list
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
    }
    // Broadcast custom multicast delegate
    if (LastSessionSearch->SearchResults.Num() <= 0){
        MultiplayerOnFindSessionsComplete.Broadcast(
            TArray<FOnlineSessionSearchResult>(), // Empty array
            false
        );
    }
    else{
        MultiplayerOnFindSessionsComplete.Broadcast(
            LastSessionSearch->SearchResults,
            bWasSuccessful
        );
    }
}


void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult &SessionResult){
    // Check if SessionInterface is not valid
    if (!SessionInterface.IsValid()){ // The way to check if TSharedPtr is valid is by using the 'IsValid' function
        // Broadcast custom multicast delegate
        MultiplayerOnJoinSessionsComplete.Broadcast(
            EOnJoinSessionCompleteResult::UnknownError
        );
        return;
    }

    /*
    Join the game session
    */
	// Add JoinSessionCompleteDelegate to SessionInterface's delegate list
    JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate); // Once the session is joined, then the fallback function which bound to this delegate will be called
    // Get the world's first local player
    const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool IsJointSuccessful = SessionInterface->JoinSession(
		*LocalPlayer->GetPreferredUniqueNetId(),
		NAME_GameSession,
		SessionResult
	);
    // If sessions joint is failed
    if (!IsJointSuccessful){
        // Remove JoinSessionCompleteDelegate from the list
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
        // Broadcast custom multicast delegate
        MultiplayerOnJoinSessionsComplete.Broadcast(
            EOnJoinSessionCompleteResult::UnknownError
        );
    }
}


void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result){
    if (SessionInterface){
        // Remove JoinSessionCompleteDelegate from the list
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
    }
    // Broadcast custom multicast delegate
    MultiplayerOnJoinSessionsComplete.Broadcast(
        Result
    );
}


void UMultiplayerSessionsSubsystem::DestroySession(){
    // Check if SessionInterface is not valid
    if (!SessionInterface.IsValid()){ // The way to check if TSharedPtr is valid is by using the 'IsValid' function
        // Broadcast custom multicast delegate
        MultiplayerOnDestroySessionComplete.Broadcast(
            false
        );
        return;
    }

    /*
    Destroy the game session
    */
    // Add DestroySessionCompleteDelegate to SessionInterface's delegate list
    DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate); // Once the session is joined, then the fallback function which bound to this delegate will be called
    // Get the world's first local player
    const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    bool IsDestructionSuccessful = SessionInterface->DestroySession(
        NAME_GameSession
    );
    // If sessions destruction is failed
    if (!IsDestructionSuccessful){
        // Remove DestroySessionCompleteDelegate from the list
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
        // Broadcast custom multicast delegate
        MultiplayerOnDestroySessionComplete.Broadcast(
            false
        );
    }
}


void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful){
    if (SessionInterface){
        // Remove DestroySessionCompleteDelegate from the list
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
    }
    // If new session creation is needed
    if (bWasSuccessful && bCreateSessionOnDestroy){
        bCreateSessionOnDestroy = false;
        CreateSession(LastNumPublicConnections, LastMatchType);
    }
    // Broadcast custom multicast delegate
    MultiplayerOnDestroySessionComplete.Broadcast(
        bWasSuccessful
    );
}


void UMultiplayerSessionsSubsystem::StartSession(){

}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful){
}