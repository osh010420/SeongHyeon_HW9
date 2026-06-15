// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChatGameModeBase.generated.h"

class AChatPlayerController;
struct FTimerHandle;
/**
 * 
 */
UCLASS()
class HW9_API AChatGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	void PrintChatMessage(AChatPlayerController* ChatPlayerController,const FString& Message);
	
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	FString GenerateSecretNumber();
	
	bool IsGuessNumber(const FString& NumberString);
	
	FString JudgeResult(const FString& SecretNumberString, const FString& GuessNumberString);
	
	void IncreaseGuessCount(AChatPlayerController* ChatPlayerController);
	
	void Reset();
	
	void JudgeGame(AChatPlayerController* InChattingPlayerController, int InStrikeCount);
	
	//도전과제
	void StartTurn();
	void EndTurn();
	bool bIsPlayerTurn(AChatPlayerController* InChattingPlayerController);
	
protected:
	FString SecretNumber;
	
	TArray<TObjectPtr<AChatPlayerController>> AllPlayerControllers;
	
	float TurnDuration = 30.0f;
	FTimerHandle TimerHandle;
	
	int32 CurrentPlayerIndex = 0;
	
private:
	void OnTurnTimerTick();
};
