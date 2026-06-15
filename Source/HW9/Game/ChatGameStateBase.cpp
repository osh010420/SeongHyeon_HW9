// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ChatGameStateBase.h"

#include "Controller/ChatPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AChatGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			AChatPlayerController* CPC = Cast<AChatPlayerController>(PC);
			if (IsValid(CPC))
			{
				FString NotificationString = InString + TEXT(" has Joined The Game");
				CPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

AChatGameStateBase::AChatGameStateBase()
{
	bReplicates = true;
}

void AChatGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemaningTime);
	DOREPLIFETIME(ThisClass, CurrentTurnPlayerIndex);
}
