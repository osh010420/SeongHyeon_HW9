// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ChatGameModeBase.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "ChatGameStateBase.h"
#include "Controller/ChatPlayerState.h"
#include "Controller/ChatPlayerController.h"

void AChatGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	SecretNumber = GenerateSecretNumber();
}

void AChatGameModeBase::PrintChatMessage(AChatPlayerController* ChatPlayerController, const FString& Message)
{
	int32 index = Message.Len() - 3;
	FString GuessMessage = Message.RightChop(index);
    
	AChatPlayerState* PS = ChatPlayerController->GetPlayerState<AChatPlayerState>();

	if (IsGuessNumber(GuessMessage))
	{
		if (!bIsPlayerTurn(ChatPlayerController))
		{
			ChatPlayerController->ClientRPCPrintChat(TEXT("지금은 당신의 턴이 아닙니다."));
			return;
		}
		
		FString JudgeResultString = JudgeResult(SecretNumber, GuessMessage);
		IncreaseGuessCount(ChatPlayerController);
        
		if (IsValid(PS))
			PS->bHasGuessTurn = true;
		
		FString PlayerInfo = IsValid(PS) ? PS->GetPlayerInfo() : TEXT("Unknown");

		for (TActorIterator<AChatPlayerController> It(GetWorld()); It; ++It)
		{
			AChatPlayerController* PC = *It;
			if (IsValid(PC))
			{
				FString Combined = PlayerInfo + TEXT(" : ") + GuessMessage + TEXT(" -> ") + JudgeResultString;
				PC->ClientRPCPrintChat(Combined);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(ChatPlayerController, StrikeCount);
		
		if (StrikeCount != 3)
		{
			EndTurn();
		}
	}
	else
	{
		FString PlayerInfo = IsValid(PS) ? PS->GetPlayerInfo() : TEXT("Unknown");

		for (TActorIterator<AChatPlayerController> It(GetWorld()); It; ++It)
		{
			AChatPlayerController* PC = *It;
			if (IsValid(PC))
			{
				PC->ClientRPCPrintChat(TEXT("다시 입력하세요"));
			}
		}
	}
}

void AChatGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	
	AChatPlayerController* PC = Cast<AChatPlayerController>(NewPlayer);
	if (IsValid(PC))
	{
		PC->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(PC);
		
		AChatPlayerState* ChatPS = PC->GetPlayerState<AChatPlayerState>();
		{
			if (IsValid(ChatPS))
			{
				ChatPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
			}
			AChatGameStateBase* GameStateBase = GetGameState<AChatGameStateBase>();
			if (IsValid(GameStateBase))
			{
				GameStateBase->MulticastRPCBroadcastLoginMessage(ChatPS->PlayerNameString);
			}
			
			if (AllPlayerControllers.Num() == 1)
			{
				StartTurn();
			}
		}
	}
	
	
	
}

FString AChatGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i=1; i<=9; ++i)
	{
		Numbers.Add(i);
	}
	
	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num>0;});
	
	FString Result;
	for (int32 i=0; i<3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num()-1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	
	return Result;
}

bool AChatGameModeBase::IsGuessNumber(const FString& NumberString)
{
	bool bCanPlay = false;
	
	do
	{
		if (NumberString.Len() != 3)
		{
			break;
		}
		
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR c : NumberString)
		{
			if (FChar::IsDigit(c) == false || c == '0')
			{
				bIsUnique = false;
				break;
			}
			
			UniqueDigits.Add(c);
		}
		if (bIsUnique == false)
		{
			break;
		}
		bCanPlay = true;
	}while (false);
	
	return bCanPlay;
}

FString AChatGameModeBase::JudgeResult(const FString& SecretNumberString, const FString& GuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;
	
	for (int32 i=0; i<3; ++i)
	{
		if (SecretNumberString[i] == GuessNumberString[i])
		{
			++StrikeCount;
		}
		else
		{
			FString PlayerGuess = FString::Printf(TEXT("%c"), GuessNumberString[i]);
			if (SecretNumberString.Contains(PlayerGuess))
			{
				++BallCount;
			}
		}
	}
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("Out");
	}
	
	return FString::Printf(TEXT("%d S %d B"), StrikeCount, BallCount);
}

void AChatGameModeBase::IncreaseGuessCount(AChatPlayerController* ChatPlayerController)
{
	AChatPlayerState* ChatPS = ChatPlayerController->GetPlayerState<AChatPlayerState>();
	if (IsValid(ChatPS))
	{
		ChatPS->GuessCurrentCount++;
	}
}

void AChatGameModeBase::Reset()
{
	SecretNumber = GenerateSecretNumber();
	CurrentPlayerIndex = 0; 
	
	for (const auto& ChatPC : AllPlayerControllers)
	{
		AChatPlayerState* ChatPS = ChatPC->GetPlayerState<AChatPlayerState>();
		if (IsValid(ChatPS))
		{
			ChatPS->GuessCurrentCount = 0;
			ChatPS->bHasGuessTurn = false;
		}
	}
	StartTurn();
}

void AChatGameModeBase::JudgeGame(AChatPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		AChatPlayerState* ChatPS = InChattingPlayerController->GetPlayerState<AChatPlayerState>();
		for (const auto& PC : AllPlayerControllers)
		{
			FString CombinedMessage = ChatPS->PlayerNameString + TEXT(" Has won the game.");
			PC->NotificationText = FText::FromString(CombinedMessage);
			
			Reset();
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& PC : AllPlayerControllers)
		{
			AChatPlayerState* ChatPS = PC->GetPlayerState<AChatPlayerState>();
			if (IsValid(ChatPS))
			{
				if (ChatPS->GuessCurrentCount < ChatPS->MaxCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (bIsDraw)
		{
			for (const auto& PC : AllPlayerControllers)
			{
				PC->NotificationText = FText::FromString(TEXT("Draw"));
				
				Reset();
			}
		}
	}
}

void AChatGameModeBase::StartTurn()
{
	if (AllPlayerControllers.Num() == 0) return;
	
	CurrentPlayerIndex = CurrentPlayerIndex % AllPlayerControllers.Num();
	
	AChatGameStateBase* ChatGS = GetGameState<AChatGameStateBase>();
	if (IsValid(ChatGS))
	{
		ChatGS->RemaningTime = TurnDuration;
		ChatGS->CurrentTurnPlayerIndex = CurrentPlayerIndex;
	}
	
	AChatPlayerController* ChatPC = AllPlayerControllers[CurrentPlayerIndex];
	if (IsValid(ChatPC))
	{
		AChatPlayerState* ChatPS = ChatPC->GetPlayerState<AChatPlayerState>();
		if (IsValid(ChatPS))
		{
			ChatPS->bHasGuessTurn = false;
		}
		
		FString Turn = ChatPS->PlayerNameString + TEXT("의 턴이 시작되었습니다.");
		for (auto& PC : AllPlayerControllers)
		{
			if (IsValid(PC))
				PC->ClientRPCPrintChat(Turn);
		}
	}
	else
	{
		return;
	}
	GetWorldTimerManager().SetTimer(
		TimerHandle, 
		this,
		&AChatGameModeBase::OnTurnTimerTick,
		1.f,
		true);
}

void AChatGameModeBase::OnTurnTimerTick()
{
	AChatGameStateBase* ChatGS = GetGameState<AChatGameStateBase>();
	if (!IsValid(ChatGS)) return;
	
	ChatGS->RemaningTime -= 1.f;
	
	if (ChatGS->RemaningTime <= 0.f)
	{
		EndTurn();
	}
}

void AChatGameModeBase::EndTurn()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

	if (AllPlayerControllers.IsValidIndex(CurrentPlayerIndex))
	{
		AChatPlayerController* PC = AllPlayerControllers[CurrentPlayerIndex];
		if (IsValid(PC))
		{
			AChatPlayerState* PS = PC->GetPlayerState<AChatPlayerState>();
			
			if (IsValid(PS) && PS->bHasGuessTurn == false)
			{
				IncreaseGuessCount(PC);

				FString TimeoutMsg = PS->GetPlayerInfo() + TEXT(" : 시간 초과!");
				for (auto& Controller : AllPlayerControllers)
				{
					if (IsValid(Controller))
						Controller->ClientRPCPrintChat(TimeoutMsg);
				}
			}
		}
	}
	
	CurrentPlayerIndex = (CurrentPlayerIndex + 1) % AllPlayerControllers.Num();
	StartTurn();
}

bool AChatGameModeBase::bIsPlayerTurn(AChatPlayerController* InChattingPlayerController)
{
	if (!AllPlayerControllers.IsValidIndex(CurrentPlayerIndex)) return false;
	return AllPlayerControllers[CurrentPlayerIndex] == InChattingPlayerController;
}
