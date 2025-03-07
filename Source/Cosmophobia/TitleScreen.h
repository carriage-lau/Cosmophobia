#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleScreen.generated.h"

UCLASS()
class UTitleScreenWidget : public UUserWidget {
    GENERATED_BODY()
public:
    SLATE_BEGIN_ARGS(UTitleScreenWidget)
    {
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<SVerticalBox> TitleScreen;
    TSharedPtr<STextBlock> TitleText;
    TSharedPtr<SButton> StartButton;
    TSharedPtr<SButton> ExitButton;
    FReply OnStartButtonClicked();
    FReply OnExitButtonClicked();
};