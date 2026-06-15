// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ChatPlayerState.h"
#include "Net/UnrealNetwork.h"

AChatPlayerState::AChatPlayerState()
: PlayerNameString(TEXT("None")),
MaxCount(3),
GuessCurrentCount(0)
{
	bReplicates = true;
}

void AChatPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, GuessCurrentCount);
	DOREPLIFETIME(ThisClass, bHasGuessTurn);
}

FString AChatPlayerState::GetPlayerInfo()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(GuessCurrentCount) 
	+ TEXT(" / ") + FString::FromInt(MaxCount) + TEXT(")");
	return PlayerInfoString;
}
