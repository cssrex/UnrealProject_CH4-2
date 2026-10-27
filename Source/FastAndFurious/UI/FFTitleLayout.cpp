
#include "FFTitleLayout.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerController/FFTitlePlayerController.h"

UFFTitleLayout::UFFTitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFFTitleLayout::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UFFTitleLayout::OnPlayButtonClicked()
{
	AFFTitlePlayerController* PlayerController = GetOwningPlayer<AFFTitlePlayerController>();
	if (IsValid(PlayerController) == true)
	{
		FText ServerIP = ServerIPEditableText->GetText();
		PlayerController->JoinServer(ServerIP.ToString());
	}
}

void UFFTitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
