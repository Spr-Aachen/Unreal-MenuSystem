// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MultiplayerSessionsSubsystem.h"

// Header files with '.generated' should be put in the end
#include "Menu.generated.h"


UCLASS()
class MENUSYSTEM_API UMenu : public UUserWidget{
	GENERATED_BODY()

private:
	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem *MultiplayerSessionsSubsystem; // Or use forward declaration if not willing to include header file

	// Number of public connections
	int32 NumPublicConnections{4};
	// Match type
	FString MatchType{TEXT("FreeForAll")};
	// Lobby path
	FString PathToLobby{TEXT("")}; // Initialize it with empty string so that we can give this a valid value later

	// Link to the HostButton that exists on widget blueprint
	UPROPERTY(meta = (BindWidget)) // While using BindWidget meta specifier, note that the variable has to have the same name as the button on widget blueprint
	class UButton *HostButton; // Forward declare so there's no need to include the buttons' header file

	// Callback function which will be called when HostButton is clicked
	UFUNCTION() // Because we're binding this to an OnClickedEvent or delegate that exists in the new button in class
	void HostButtonClicked();

	// Link to the JoinButton that exists on widget blueprint
	UPROPERTY(meta = (BindWidget))
	UButton *JoinButton;

	// Callback function which will be called when JoinButton is clicked
	UFUNCTION() // Because we're binding this to an OnClickedEvent or delegate that exists in the new button in class
	void JoinButtonClicked();

protected:
	// Override the inherited 'Initialize' virtual function on UUserWidget class to bind callback functions, this is because that `Initialize` is kind of like the constructor which will create the widget
	virtual bool Initialize() override;

protected:
	/*
	Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	*/
	// Callback function which will be called when delegate is broadcast
	UFUNCTION() // Because we're binding this to a dynamic multicast delegate
	void OnCreateSession(bool bWasSuccessful);
	// Callback function which will be called when delegate is broadcast
	void OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccessful);
	// Callback function which will be called when delegate is broadcast
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	// Callback function which will be called when delegate is broadcast
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	// Callback function which will be called when delegate is broadcast
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

public:
	// Blueprint callable function to setup menu
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby"))); // By giving default values, inputs won't be requirements

private:
	// Function to remove the widget from the viewport and reset the input mode
	void MenuTearDown();

protected:
	// Override the inherited 'NativeDestruct' virtual function on UUserWidget class to call MenuTearDown function, this function gets called when the level is removed form the world which happens when traveling to another level
	virtual void NativeDestruct() override;
};
