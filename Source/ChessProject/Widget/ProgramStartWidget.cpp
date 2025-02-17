// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgramStartWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"

#include <ChessProject/Network/MyNetworkSubsystem.h>

void UProgramStartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	StartBtn = Cast<UButton>(GetWidgetFromName(TEXT("StartBtn")));
	if (StartBtn)
	{
		StartBtn->OnClicked.AddUniqueDynamic(this, &UProgramStartWidget::OnTryProgramStart);
	}

	InputID = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("InputID")));
}

void UProgramStartWidget::OnTryProgramStart()
{
	UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem)
	{
		FText UserInputID = InputID->GetText();

		if (MyNetworkSystem->IsConnected())
		{
			MyNetworkSystem->RequestProgramStart(UserInputID.ToString());
		}
	}
}
