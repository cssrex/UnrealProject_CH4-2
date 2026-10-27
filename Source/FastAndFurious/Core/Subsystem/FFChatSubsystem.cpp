
#include "FFChatSubsystem.h"

void UFFChatSubsystem::AddChatMessage(const FString& InSender, const FString& InMessage)
{
	if (ChatLog.Num() > MaxChatLog)
	{
		ChatLog.RemoveAt(0);
	}

	ChatLog.Emplace(InSender, InMessage);
}
