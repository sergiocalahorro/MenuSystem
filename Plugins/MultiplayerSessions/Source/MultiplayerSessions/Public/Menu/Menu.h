// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

// MultiplayerSessions
#include "Settings/MultiplayerSessionSettings.h"

#include "Menu.generated.h"

// Forward declarations - Unreal Engine
class UButton;

// Forward declarations - MultiplayerSessions
class UMultiplayerSessionsSubsystem;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

#pragma region OVERRIDES

protected:

	/** Initialize widget */
	virtual bool Initialize() override;

	/** Destruct widget */
	virtual void NativeDestruct() override;

#pragma endregion OVERRIDES

#pragma region MENU
	
public:

	/** Setup menu */
	UFUNCTION(BlueprintCallable)
	void SetupMenu(const FMultiplayerSessionSettings& InMultiplayerSessionSettings);

private:

	/** Callback for HostButton's OnClicked event */
	UFUNCTION()
	void HostButtonClicked();

	/** Callback for JoinButton's OnClicked event */
	UFUNCTION()
	void JoinButtonClicked();

	/** Callback for QuitButton's OnClicked event */
	UFUNCTION()
	void QuitButtonClicked();

	/** Remove menu */
	UFUNCTION()
	void RemoveMenu();

private:

	/** Button used for hosting the game session */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;
	
	/** Button used for joining an already existing game session */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	/** Button used for quitting the game */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

#pragma endregion MENU

#pragma region SESSION

protected:

	/** Callback called when the multiplayer session creation is complete */
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	/** Callback called when the multiplayer sessions finding is complete */
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	/** Callback called when the multiplayer session join is complete */
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	/** Callback called when the multiplayer session start is complete */
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	/** Callback called when the multiplayer session destruction is complete */
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:
	
	/** Subsystem designed to handle all online session functionality */
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	/** Settings used for the multiplayer session */
	UPROPERTY()
	FMultiplayerSessionSettings MultiplayerSessionSettings;

#pragma endregion SESSION
	
};
