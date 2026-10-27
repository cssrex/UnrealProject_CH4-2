
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FFPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AFFPawn;
class UFFUI;

UCLASS(abstract)
class FASTANDFURIOUS_API  AFFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	friend class UFFChatUI;
	friend class AFFGameMode;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void Tick(float Delta) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientRPCChatMessage(const FString& Sender, const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientRPCChatLog(const TArray<FChatData>& ChatLog);

protected:
	void OpenChat();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenChatAction;

	UPROPERTY(Replicated)
	TObjectPtr<AFFPawn> VehiclePawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UFFUI> VehicleUIClass;

	TObjectPtr<UFFUI> VehicleUI;




};
