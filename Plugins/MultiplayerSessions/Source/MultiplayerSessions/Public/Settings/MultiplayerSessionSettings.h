// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

#include "MultiplayerSessionSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FMultiplayerSessionSettings
{
	GENERATED_USTRUCT_BODY()

public:

	/** Number of public connections allowed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPublicConnections = 4;

	/** Maximum number of search results */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSearchResults = 10000;

	/** Match type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchType = FString("FreeForAll");

	/** Path to lobby map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PathToLobby;
};
