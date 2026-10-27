
#include "UI/FFChatUI.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Player/PlayerController/FFPlayerController.h"
#include "UI/FFChatUI.h"


void UFFChatUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UFFChatUI::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &UFFChatUI::OnChatInputTextCommitted);
	}
}

void UFFChatUI::NativeDestruct()
{
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UFFChatUI::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &UFFChatUI::OnChatInputTextCommitted);
	}

	Super::NativeDestruct();
}

void UFFChatUI::OnNewChatMessage(const FString& Sender, const FString& Message)
{
	FString NewMessage = Sender + TEXT(": ") + Message;
	FString OldMessage = MultiLineEditableTextBox_ChatLog->GetText().ToString();
	FText FinalMessage = FText::FromString(OldMessage + '\n' + NewMessage);
	MultiLineEditableTextBox_ChatLog->SetText(FinalMessage);
}

void UFFChatUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		const FString ChatMessage = Text.ToString();
		EditableTextBox_ChatInput->SetText(FText::GetEmpty());
		if (ChatMessage.IsEmpty() == false)
		{
			AFFPlayerController* PC = GetOwningPlayer<AFFPlayerController>();
			if (IsValid(PC))
			{
				if (!PC->IsLocalController())
				{
					return;
				}

				// 실제 채팅 코드
				PC->ServerRPCChatMessage(ChatMessage);
			}
		}
	}
}
