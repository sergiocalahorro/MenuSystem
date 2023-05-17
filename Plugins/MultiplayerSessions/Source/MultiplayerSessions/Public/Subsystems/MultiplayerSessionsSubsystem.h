// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

// MultiplayerSessions
#include "Settings/MultiplayerSessionSettings.h"

#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionCompleteSignature, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsCompleteSignature, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionCompleteSignature, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionCompleteSignature, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionCompleteSignature, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region INITIALIZATION
	
public:

	/** Constructor */
	UMultiplayerSessionsSubsystem();

#pragma endregion INITIALIZATION

#pragma region SESSION

public:
	
	/** Create session */
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	
	/** Find sessions */
	void FindSessions(int32 MaxSearchResults);

	/** Join session */
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	
	/** Start session */
	void StartSession();

	/** Destroy session */
	void DestroySession();

protected:

	/** Callback bound to the delegate used for creating the session is completed */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Callback bound to the delegate used for finding sessions is completed */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/** Callback bound to the delegate used for joining the session is completed */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** Callback bound to the delegate used for starting the session is completed */
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Callback bound to the delegate used for destroying the session is completed */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

public:

	/** Delegate called when the multiplayer session creation is complete */
	FMultiplayerOnCreateSessionCompleteSignature MultiplayerOnCreateSessionCompleteDelegate;

	/** Delegate called when the multiplayer sessions' finding is complete */
	FMultiplayerOnFindSessionsCompleteSignature MultiplayerOnFindSessionsCompleteDelegate;

	/** Delegate called when the multiplayer sessions joining is complete */
	FMultiplayerOnJoinSessionCompleteSignature MultiplayerOnJoinSessionCompleteDelegate;

	/** Delegate called when the multiplayer session start is complete */
	FMultiplayerOnStartSessionCompleteSignature MultiplayerOnStartSessionCompleteDelegate;

	/** Delegate called when the multiplayer sessions destruction is complete */
	FMultiplayerOnDestroySessionCompleteSignature MultiplayerOnDestroySessionCompleteDelegate;
	
private:

	/** Pointer to the online session interface */
	IOnlineSessionPtr SessionInterface;

	/** Last online session's settings */
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	/** Last online session search */
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	/** Delegate called by the Online Session Interface when creating the session is completed */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;

	/** Handle for CreateSessionCompleteDelegate */
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	/** Delegate called by the Online Session Interface when finding sessions is completed */
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;

	/** Handle for FindSessionsCompleteDelegate */
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	/** Delegate called by the Online Session Interface when joining the session is completed */
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	/** Handle for JoinSessionCompleteDelegate */
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	/** Delegate called by the Online Session Interface when starting the session is completed */
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;

	/** Handle for StartSessionCompleteDelegate */
	FDelegateHandle StartSessionCompleteDelegateHandle;

	/** Delegate called by the Online Session Interface when destroying the session is completed */
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;

	/** Handle for DestroySessionCompleteDelegate */
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	/** Tracks whether session can be created on destroy */
	bool bCreateSessionOnDestroy = false;

	/** Last multiplayer session settings */
	FMultiplayerSessionSettings LastMultiplayerSessionSettings;

#pragma endregion SESSION
};
