#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleScreenWidget.generated.h"

UCLASS()
class UTitleScreenWidget : public UUserWidget{
    public:
        SLATE_BEGIN_ARGS(UTitleScreenWidget)
        {}
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs);
        
    private:
        TSharedPtr<SVerticalBox> TitleScreen;
        TSharedPtr<STextBlock> TitleText;
        TSharedPtr<SButton> StartButton;
        TSharedPtr<SButton> ExitButton;
        FReply OnStartButtonClicked();
        FReply OnExitButtonClicked();
}