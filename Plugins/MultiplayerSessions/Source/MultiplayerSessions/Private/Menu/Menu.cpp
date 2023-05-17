// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/Menu.h"

// Unreal Engine
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/KismetSystemLibrary.h"

// MultiplayerSessions
#include "Subsystems/MultiplayerSessionsSubsystem.h"

#pragma region OVERRIDES

/** Initialize widget */
bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	// Bind buttons' callbacks
	if (HostButton)
	{
		HostButton->OnClicked.AddUniqueDynamic(this, &UMenu::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddUniqueDynamic(this, &UMenu::JoinButtonClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddUniqueDynamic(this, &UMenu::QuitButtonClicked);
	}
	
	return true;
}

/** Destruct widget */
void UMenu::NativeDestruct()
{
	RemoveMenu();
	Super::NativeDestruct();
}

#pragma endregion OVERRIDES

#pragma region MENU

/** Setup menu */
void UMenu::SetupMenu(const FMultiplayerSessionSettings& InMultiplayerSessionSettings)
{
	// Set multiplayer session settings
	MultiplayerSessionSettings = InMultiplayerSessionSettings;
	MultiplayerSessionSettings.PathToLobby = FString::Printf(TEXT("%s?listen"), *InMultiplayerSessionSettings.PathToLobby);

	// Show widget
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	// Setup input
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// Initialize multiplayer sessions' subsystem
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	
	// Setup callbacks
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionCompleteDelegate.AddUniqueDynamic(this, &UMenu::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsCompleteDelegate.AddUObject(this, &UMenu::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionCompleteDelegate.AddUObject(this, &UMenu::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionCompleteDelegate.AddUniqueDynamic(this, &UMenu::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionCompleteDelegate.AddUniqueDynamic(this, &UMenu::OnDestroySession);
	}
}

/** Remove menu */
void UMenu::RemoveMenu()
{
	RemoveFromParent();
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

/** Callback for HostButton's OnClicked event */
void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(MultiplayerSessionSettings.NumPublicConnections, MultiplayerSessionSettings.MatchType);
	}
}

/** Callback for JoinButton's OnClicked event */
void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(MultiplayerSessionSettings.MaxSearchResults);
	}
}

/** Callback for QuitButton's OnClicked event */
void UMenu::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

#pragma endregion MENU

#pragma region SESSION

/** Callback called when the multiplayer session creation is complete */
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	// Debug
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			INDEX_NONE,
			15.f,
			bWasSuccessful ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("Session created - bWasSuccessful: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"))
		);
	}
	
	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(MultiplayerSessionSettings.PathToLobby);
		}
	}
	else
	{
		HostButton->SetIsEnabled(true);
	}
}

/** Callback called when the multiplayer sessions finding is complete */
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}
	
	for (const FOnlineSessionSearchResult& Result : SessionResults)
	{
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		if (MatchType.Equals(MultiplayerSessionSettings.MatchType))
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
		}
	}

	if (!bWasSuccessful || SessionResults.IsEmpty())
	{
		JoinButton->SetIsEnabled(true);
	}
}

/** Callback called when the multiplayer session join is complete */
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	/** Initialize online subsystem */
	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

/** Callback called when the multiplayer session start is complete */
void UMenu::OnStartSession(bool bWasSuccessful)
{
	
}

/** Callback called when the multiplayer session destruction is complete */
void UMenu::OnDestroySession(bool bWasSuccessful)
{
	
}

#pragma endregion SESSION
