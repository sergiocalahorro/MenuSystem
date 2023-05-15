// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystemCharacter.generated.h"

// Forward declarations - Unreal Engine
class IOnlineSession;

UCLASS(config=Game)
class AMenuSystemCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AMenuSystemCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region SESSION
	
protected:

	/** Create game session */
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();
	
	/** Function bound to delegate called when session's creation is complete */
	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Join game session */
	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	/** Function bound to delegate called when finding sessions is complete */
	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);

private:

	/** Initialize online subsystem */
	UFUNCTION()
	void InitializeOnlineSubsystem();

public:

	/** Pointer to the online session interface */
	IOnlineSessionPtr OnlineSessionInterface;

private:

	/** Delegate called when session's creation is complete */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;

	/** Delegate called when finding sessions is complete */
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;

	/** Session search parameters */
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
#pragma endregion SESSION
	
};
