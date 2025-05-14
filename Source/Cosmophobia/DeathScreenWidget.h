#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

UCLASS(Blueprintable)
class COSMOPHOBIA_API UDeathScreenWidget : public UUserWidget {
    GENERATED_BODY()

protected:
    // Triggers for initialization and keydown
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    // Handling the fade out
    UFUNCTION()
    void HandleFadeOutFinished();

    // Animations
    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* FadeIn;

    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* FadeOut;

    bool bFadingOut = false;
};
