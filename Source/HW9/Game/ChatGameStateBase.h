// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChatGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class HW9_API AChatGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AChatGameStateBase();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InString = FString(TEXT("???")));
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float RemaningTime = 0.f;
	
	UPROPERTY(Replicated)
	int32 CurrentTurnPlayerIndex = 0;
};
