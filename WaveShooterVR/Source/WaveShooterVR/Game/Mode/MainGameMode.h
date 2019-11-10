#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#pragma endregion

#pragma region project
#include "Utiliy/Structs.h"
#include "MainGameMode.generated.h"
#pragma endregion

UCLASS()
/// <summary>
/// main game mode
/// </summary>
class WAVESHOOTERVR_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	AMainGameMode();
#pragma endregion

#pragma region public override function
	/// <summary>
	/// update every frame
	/// </summary>
	/// <param name="DeltaTime">time since last frame</param>
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy")
	/// <summary>
	/// waves array
	/// </summary>
	TArray<FEnemyWave> Waves;
#pragma endregion

protected:
#pragma region protected override function
	/// <summary>
	/// called at begin play
	/// </summary>
	virtual void BeginPlay() override;
#pragma endregion

private:
#pragma region private primitive variable
	/// <summary>
	/// wave started
	/// </summary>
	bool m_waveStarted = false;

	/// <summary>
	/// spawn timer
	/// </summary>
	float m_spawnTime = 0.0f;
#pragma endregion

#pragma region private variable
	/// <summary>
	/// all spawn locations
	/// </summary>
	TArray<FVector> m_spawnPoints;
#pragma endregion
};