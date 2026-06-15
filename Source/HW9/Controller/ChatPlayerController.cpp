// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ChatPlayerController.h"

#include "ChatPlayerState.h"
#include "HW9.h"
#include "UI/ChatWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ChatGameModeBase.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

AChatPlayerController::AChatPlayerController()
{
	bReplicates = true;
}

void AChatPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, NotificationText);
	DOREPLIFETIME(ThisClass, bHasTurn);
}

void AChatPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}
	
	FInputModeUIOnly InputModeUI;
	SetInputMode(InputModeUI);
	
	if (IsValid(ChatInputWidget))
	{
		ChatInputWidgetInstance = CreateWidget<UChatWidget>(this, ChatInputWidget);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(NotificationWidget))
	{
		NotificationWidgetInstance = CreateWidget<UUserWidget>(this, NotificationWidget);
		if (IsValid(NotificationWidgetInstance))
		{
			NotificationWidgetInstance->AddToViewport();
		}
	}
	
	
}

void AChatPlayerController::SetChatMessageString(const FString& Message)
{
	ChatMessageString = Message;
    
	if (IsLocalController())
	{
		ServerRPCPrintChat(Message);
	}
}

void AChatPlayerController::PrintChatMessageString(const FString& Message)
{
	HW9::MyPrintString(this, Message, 10.0f);
}

void AChatPlayerController::ClientRPCPrintChat_Implementation(const FString& Message)
{
	PrintChatMessageString(Message);
}

void AChatPlayerController::ServerRPCPrintChat_Implementation(const FString& Message)
{
	// for (TActorIterator<AChatPlayerController> It(GetWorld()); It; ++It)
	// {
	// 	AChatPlayerController* PlayerController = *It;
	// 	if (IsValid(PlayerController) == true)
	// 	{
	// 		PlayerController->ClientRPCPrintChat(Message);
	// 	}
	// }
	
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM))
	{
		AChatGameModeBase* ChatGM = Cast<AChatGameModeBase>(GM);
		if (IsValid(ChatGM))
		{
			ChatGM->PrintChatMessage(this, Message);
		}
	}
}
