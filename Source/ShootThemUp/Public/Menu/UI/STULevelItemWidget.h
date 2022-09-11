// Shoot Them up Game, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUCoreTypes.h"
#include "STULevelItemWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class SHOOTTHEMUP_API USTULevelItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    FOnLevelSelectedSignature OnLevelSelected;

	void SetLevelData(const FLevelData& Data); 
	void SetSelected(bool IsSelected);
    FLevelData GetLevelData() { return LevelData; };

protected:
    UPROPERTY(meta = (BindWidget))
	UButton* LevelSelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* LevelImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* FrameImage;
	
	virtual void NativeOnInitialized() override;

private:
    FLevelData LevelData;

	UFUNCTION()
    void OnLevelItemClicked();
};
