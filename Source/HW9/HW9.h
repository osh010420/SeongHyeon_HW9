// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class HW9
{
public:
	static void MyPrintString(const AActor* InworldContextActor, const FString& InString, 
		float InTimeToDisplay = 1.0f, FColor InColor = FColor::Cyan)
	{
		if (IsValid(GEngine) && IsValid(InworldContextActor))
		{
			//클라이언트이거나 리슨서버(화면이 필요한 경우)
			if (InworldContextActor->GetNetMode() == NM_Client || InworldContextActor->GetNetMode() == NM_ListenServer)
			{
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
			}
			// 데디케이티드 서버일 경우
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *InString);
			}
		}
	}
	
	static FString GetNetModeString(const AActor* InworldContextActor)
	{
		FString NetModeString = TEXT("None");
		
		if (IsValid(InworldContextActor))
		{
			if (InworldContextActor->GetNetMode() == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else
			{
				if (InworldContextActor->GetNetMode() == NM_Standalone)
				{
					NetModeString = TEXT("Standalone");
				}
				else
				{
					NetModeString = TEXT("Server");
				}
			}
		}
		return NetModeString;
	}
	
	static FString GetRoleString(const AActor* InworldContextActor)
	{
		FString RoleString = TEXT("None");
		if (IsValid(InworldContextActor))
		{
			FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InworldContextActor->GetLocalRole());
			FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InworldContextActor->GetRemoteRole());
			
			RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
		}
		return RoleString;
	}
};