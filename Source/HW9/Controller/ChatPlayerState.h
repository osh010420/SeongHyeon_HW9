// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ChatPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HW9_API AChatPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AChatPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FString GetPlayerInfo();
public:
	UPROPERTY(Replicated)
	FString PlayerNameString;
	
	UPROPERTY(Replicated)
	int32 GuessCurrentCount;
	
	UPROPERTY(Replicated)
	int32 MaxCount;
	
	UPROPERTY(Replicated)
	bool bHasGuessTurn = false;
};
