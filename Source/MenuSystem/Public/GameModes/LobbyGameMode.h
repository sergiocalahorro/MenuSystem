// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "LobbyGameMode.generated.h"

UCLASS()
class MENUSYSTEM_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

#pragma region OVERRIDES
	
public:

	/** Called after a successful login. This is the first place it is safe to call replicated functions on the PlayerController */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	virtual void Logout(AController* Exiting) override;

#pragma endregion OVERRIDES
	
};
