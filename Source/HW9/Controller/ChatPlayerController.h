// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatPlayerController.generated.h"

class UChatWidget;
/**
 * 
 */
UCLASS()
class HW9_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AChatPlayerController();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	void SetChatMessageString(const FString& Message);
	
	void PrintChatMessageString(const FString& Message);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChat(const FString& Message);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChat(const FString& Message);
	

	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatWidget> ChatInputWidget;
	UPROPERTY()
	TObjectPtr<UChatWidget> ChatInputWidgetInstance;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationWidgetInstance;
	
	FString ChatMessageString;
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	
	UPROPERTY(Replicated)
	bool bHasTurn = false;
};
