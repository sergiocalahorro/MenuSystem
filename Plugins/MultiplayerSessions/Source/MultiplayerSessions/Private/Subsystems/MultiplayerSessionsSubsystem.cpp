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
		bCreateSessionOnDestroy = true;
		LastMultiplayerSessionSettings.NumPublicConnections = NumPublicConnections;
		LastMultiplayerSessionSettings.MatchType = MatchType;
		DestroySession();
	}

	// Add delegate to list of delegates to call on create session complete, and store its handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Setup session's settings
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
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
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnStartSessionCompleteDelegate.Broadcast(false);
		return;
	}

	// Add delegate to list of delegates to call on start session complete, and store its handle
	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	// Start session
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		// Clear delegate handle if starting session failed
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		MultiplayerOnStartSessionCompleteDelegate.Broadcast(false);
	}
}

/** Destroy session */
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionCompleteDelegate.Broadcast(false);
		return;
	}

	// Add delegate to list of delegates to call on start session complete, and store its handle
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// Destroy session
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		// Clear delegate handle if destroying session failed
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionCompleteDelegate.Broadcast(false);
	}
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
	// Clear delegate handle
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnJoinSessionCompleteDelegate.Broadcast(Result);
}

/** Callback bound to the delegate used for starting the session is completed */
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface)
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnStartSessionCompleteDelegate.Broadcast(bWasSuccessful);
}

/** Callback bound to the delegate used for destroying the session is completed */
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// Create session after it was successfully destroyed, with appropriate settings
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastMultiplayerSessionSettings.NumPublicConnections, LastMultiplayerSessionSettings.MatchType);
	}

	MultiplayerOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
}

#pragma endregion SESSION