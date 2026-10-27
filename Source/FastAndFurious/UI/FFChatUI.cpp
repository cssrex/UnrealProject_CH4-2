
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

void UFFChatUI::FocusChatInput()
{
	APlayerController* PC = GetOwningPlayer();

	if (!IsValid(PC) || !IsValid(EditableTextBox_ChatInput))
	{
		return;
	}

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(EditableTextBox_ChatInput->TakeWidget());

	PC->SetInputMode(InputMode);
	EditableTextBox_ChatInput->SetUserFocus(PC);
}

void UFFChatUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter)
	{
		return;
	}

	const FString ChatMessage = Text.ToString().TrimStartAndEnd();

	EditableTextBox_ChatInput->SetText(FText::GetEmpty());

	AFFPlayerController* PC = GetOwningPlayer<AFFPlayerController>();
	if (!IsValid(PC) || !PC->IsLocalController())
	{
		return;
	}

	if (!ChatMessage.IsEmpty())
	{
		PC->ServerRPCChatMessage(ChatMessage);
	}

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = false;
}
