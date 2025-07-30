// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"


void UMenu::HostButtonClicked()
{
    // Disable HostButton
    HostButton->SetIsEnabled(false);
    /* Create session */
    // Check if MultiplayerSessionsSubsystem pointer is valid
    if (MultiplayerSessionsSubsystem)
    {
        // Call CreateSession function
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
    }    
}


void UMenu::JoinButtonClicked()
{
    // Disable JoinButton
    JoinButton->SetIsEnabled(false);
    /* Find an active session */
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSessions(
            10000 // Max session search results
        );
    }
}


// Exit from this function with false if the super version returns false, otherwise bind callbacks and return true
bool UMenu::Initialize()
{
    // Call the super version
    bool IsInitializationSuccessful = Super::Initialize();
    // If initialization is failed
    if (!IsInitializationSuccessful)
    {
        return false;
    }
    /* Bind callbacks to buttons */
    else
    {
        if (HostButton)
        {
            HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
        }
        if (JoinButton)
        {
            JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
        }
        return true;
    }
}


void UMenu::OnCreateSession(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // Print confirmation on the screen
        if (GEngine)
        {
            // Show debug message
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Green,
                FString(TEXT("Session created successfully!"))
            );
        }
        /* Travel to lobby level and open it as a listen server */
        UWorld *World = GetWorld();
        if (World)
        {
            World->ServerTravel(
				FString(PathToLobby)
            );
        }
    }
    else
    {
        // Print confirmation on the screen
        if (GEngine)
        {
            // Show debug message
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Red,
                FString(TEXT("Failed to create session!"))
            );
        }
        // Enable HostButton
        HostButton->SetIsEnabled(true);
    }
}


void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccessful)
{
    // If the MultiplayerSessionsSubsystem pointer is not null
    if (MultiplayerSessionsSubsystem == nullptr)
    {
        return;
    }
    for (auto Result : SessionResults)
    {
        FString SettingsValue;
        // Check if it has a matching key value pair that we are looking for
        Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
        // If it's the correct match type
        if (SettingsValue == MatchType)
        {
            // Call JoinSession
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }
    // Enable JoinButton if failed to find session or no session is found
    if (!bWasSuccessful || SessionResults.Num() == 0)
    {
        JoinButton->SetIsEnabled(true);
    }
}


void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    /* Get session interface */
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        // Make a local pointer for session interface
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        /* If OnlineSessionInterface is valid */
        if (SessionInterface.IsValid()) // The way to check if TSharedPtr is valid is by using the 'IsValid' function
        {
            FString Address;
            bool AddressExist = SessionInterface->GetResolvedConnectString(NAME_GameSession, Address); // This function will fill in that string with the address we need
            // Call ClientTravel
            APlayerController *PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(
                    Address,
                    ETravelType::TRAVEL_Absolute
                );
            }
        }
    }
    // Enable JoinButton if failed to join session
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        JoinButton->SetIsEnabled(true);
    }
}


void UMenu::OnDestroySession(bool bWasSuccessful)
{
}


void UMenu::OnStartSession(bool bWasSuccessful)
{
}


void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    /* Set member variables with inputs */
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath); // 

    // Add the widget to viewport
    AddToViewport();
    // Set the widget's visibility to visible
    SetVisibility(ESlateVisibility::Visible);
    // Set the widget to be focusable
    SetIsFocusable(true); // Direct access to bIsFocusable is deprecated, using SetIsFocusable function instead

    /* Set the input mode via player controller */
    // Get World
    UWorld *World = GetWorld();
    // If World is valid
    if (World)
    {
        // Get the World's first player controller
        APlayerController *PlayerController = World->GetFirstPlayerController();
        // If player controller exists
        if (PlayerController)
        {
            // That way once our widget is created and we're calling menu setup, we'll set the input mode so that we're not applying input to any pawns in the world but just focus on the UI
            FInputModeUIOnly InputModeData; // FInputModeUIOnly inherits from the base class FInputModeDataBase
            // Specify to focus on this widget
            InputModeData.SetWidgetToFocus(TakeWidget());
            // Specify to not lock the mouse cursor to the viewport and that way we can actually move the mouse cursor off the screen
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            // Set the input mode
            PlayerController->SetInputMode(InputModeData);
            // Set the cursor to be visible
            PlayerController->SetShowMouseCursor(true);
        }
    }

    UGameInstance *GameInstance = GetGameInstance();
    if (GameInstance)
    {
        // Get subsystem and store it in MultiplayerSessionsSubsystem pointer, through this subsystem we can call functions created on subsystem class
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>(); // GetSubsystem requires a template param
    }

    if (MultiplayerSessionsSubsystem)
    {
        /* Bind callback functions to multicast delegates */
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionsComplete.AddUObject(this, &UMenu::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
    }
}


void UMenu::MenuTearDown()
{
    // Remove the widget from the viewport
    RemoveFromParent();
    /* Reset the input mode */
    // Get World
    UWorld *World = GetWorld();
    // If World is valid
    if (World)
    {
        // Get player controller from the world
        APlayerController *PlayerController = World->GetFirstPlayerController();
        /* Set input mode */
        // If PlayerController is valid
        if (PlayerController)
        {
            // That way player input / player controller will respond to our user input
            FInputModeGameOnly InputModeData;
            // Set the input mode
            PlayerController->SetInputMode(InputModeData);
            // Set the cursor to be invisible
            PlayerController->SetShowMouseCursor(false);
            
        }
    }
}


/*void UMenu::OnLevelRemovedFromWorld(ULevel *InLevel, UWorld *InWorld)
{
    MenuTearDown();
    Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}*/
void UMenu::NativeDestruct()
{
    // Remove the widget from the viewport and reset the input mode
    MenuTearDown();
    // Call the super version
    Super::NativeDestruct();
}