// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/LobbyGameMode.h"

// Unreal Engine
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

#pragma region OVERRIDES

/** Called after a successful login. This is the first place it is safe to call replicated functions on the PlayerController */
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		// Debug
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers)
			);
			
			if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
			{
				GEngine->AddOnScreenDebugMessage(
					2,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerState->GetPlayerName())
				);
			}
		}
	}
}

/** Called when a Controller with a PlayerState leaves the game or is destroyed */
void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// Debug
	const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	GEngine->AddOnScreenDebugMessage(
		1,
		60.f,
		FColor::Yellow,
		FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
	);

	if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
	{
		GEngine->AddOnScreenDebugMessage(
			2,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game!"), *PlayerState->GetPlayerName())
		);
	}
}

#pragma endregion OVERRIDES