// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ChatWidget.h"

#include "Components/EditableTextBox.h"
#include "Controller/ChatPlayerController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(EditableTextBox_ChatInput))
	{
		return;
	}
	
	if (!EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UChatWidget::NativeDestruct()
{
	if (IsValid(EditableTextBox_ChatInput) &&
		EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}

	Super::NativeDestruct();
}

void UChatWidget::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* Pc = GetOwningPlayer();
		if (IsValid(Pc))
		{
			AChatPlayerController* ChatPc = Cast<AChatPlayerController>(Pc);
			if (IsValid(ChatPc))
			{
				ChatPc->SetChatMessageString(Text.ToString());
				
				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
