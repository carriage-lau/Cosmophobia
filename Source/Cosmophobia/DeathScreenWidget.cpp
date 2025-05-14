#include "DeathScreenWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Animation/WidgetAnimation.h"

void UDeathScreenWidget::NativeConstruct() {
    Super::NativeConstruct();
    
    this -> SetKeyboardFocus();
    
    // Plays a fade in upon construction
    if(FadeIn){
        PlayAnimation(FadeIn);
    }
}

FReply UDeathScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
    // Initializes the fade out if a key is pressed.
    if(bFadingOut || !FadeOut) return FReply::Unhandled();
    
    bFadingOut = true;
    PlayAnimation(FadeOut);
    
    // Sets a timer to fade to title screen after the animation is done.
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
        float Duration = FadeOut->GetEndTime();
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UDeathScreenWidget::HandleFadeOutFinished, Duration, false);
    });

    return FReply::Handled();
}

void UDeathScreenWidget::HandleFadeOutFinished() {
    // stub
    // this is supposed to display the title screen.
}
