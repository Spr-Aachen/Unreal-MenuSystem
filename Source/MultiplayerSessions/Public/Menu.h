// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"


UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

private:
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

public:
	// Blueprint callable function to setup menu
	UFUNCTION(BlueprintCallable)
	void MenuSetup();
};
