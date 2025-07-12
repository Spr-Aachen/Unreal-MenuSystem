// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"


void UMenu::MenuSetup()
{
    // Add the widget to viewport
    AddToViewport();
    // Set the widget's visibility to visible
    SetVisibility(ESlateVisibility::Visible);
    // Be able to focus on the widget
    bIsFocusable = true;

    /* Set the input mode via player controller */
    // Get World
    UWorld *World = GetWorld();
    // Check to see if World is valid
    if (World)
    {
        // Get the World's first player controller
        APlayerController *PlayerController = World->GetFirstPlayerController();
        // Check to see if player controller exists
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
}