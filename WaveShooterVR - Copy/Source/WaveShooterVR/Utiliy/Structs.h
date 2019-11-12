#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#pragma endregion

#pragma region project include
#include "Character/Base/BaseCharacter.h"
#include "Structs.generated.h"
#pragma endregion

USTRUCT(BlueprintType)
/// <summary>
/// enemy wave struct
/// </summary>
struct FEnemyWave
{
	GENERATED_BODY()

public:
#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyWave")
	/// <summary>
	/// number of enemies
	/// </summary>
	int Count = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyWave")
	/// <summary>
	/// time until an enemy spawn in seconds
	/// </summary>
	float SpawnTime = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyWave")
	/// <summary>
	/// time until wave starts in seconds
	/// </summary>
	float WaveStartTime = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Required")
	/// <summary>
	/// enemy class to spawn
	/// </summary>
	TSubclassOf<ABaseCharacter> EnemyClass;
#pragma endregion
};