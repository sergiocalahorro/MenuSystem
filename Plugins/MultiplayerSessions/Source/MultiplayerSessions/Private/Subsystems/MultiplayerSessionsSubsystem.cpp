// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/MultiplayerSessionsSubsystem.h"

// Unreal Engine
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

#pragma region INITIALIZATION
	
/** Constructor */
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	/** Initialize online subsystem */
	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		// Debug
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				INDEX_NONE,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString())
			);
		}
	}

	// Create delegates
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
	StartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnStartSessionComplete);
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
}

#pragma endregion INITIALIZATION

#pragma region SESSION

/** Create session */
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// Destroy existing session, if any
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	// Add delegate to list of delegates to call on create session complete, and store its handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Setup session's settings
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// Create session
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
		{
			// Clear delegate handle if creating session failed
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
			MultiplayerOnCreateSessionCompleteDelegate.Broadcast(false);
		}
	}
}
	
/** Find sessions */
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// Add delegate to list of delegates to call on find sessions complete, and store its handle
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// Search session settings' setup
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Find sessions
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
		{
			// Clear delegate handle if finding sessions failed
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
			MultiplayerOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		}
	}
}

/** Join session */
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// Add delegate to list of delegates to call on join session complete, and store its handle
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// Join session
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
		{
			// Clear delegate handle if finding sessions failed
			SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
			MultiplayerOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		}
	}
}
	
/** Start session */
void UMultiplayerSessionsSubsystem::StartSession()
{
	
}

/** Destroy session */
void UMultiplayerSessionsSubsystem::DestroySession()
{
	
}

/** Callback bound to the delegate used for creating the session is completed */
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
}

/** Callback bound to the delegate used for finding sessions is completed */
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// Broadcast an empty array and failure if there are no search results
	if (LastSessionSearch->SearchResults.IsEmpty())
	{
		MultiplayerOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	
	MultiplayerOnFindSessionsCompleteDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

/** Callback bound to the delegate used for joining the session is completed */
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnJoinSessionCompleteDelegate.Broadcast(Result);
}

/** Callback bound to the delegate used for starting the session is completed */
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	
}

/** Callback bound to the delegate used for destroying the session is completed */
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	
}

#pragma endregion SESSION