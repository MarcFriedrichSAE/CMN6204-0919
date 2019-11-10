#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#pragma endregion

#pragma region project include
#include "Chracter/Base/BaseCharacter.h"
#include "Enemy.generated.h"
#pragma endregion

#pragma region forward decleration
class USplineComponent;
#pragma endregion

UCLASS()
/// <summary>
/// enemy class
/// </summary>
class WAVESHOOTERVR_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	AEnemy();
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Enemy")
	/// <summary>
	/// shot range
	/// </summary>
	float ShotRange = 250.0f;
#pragma endregion

#pragma region public override function
	/// <summary>
	/// update every frame
	/// </summary>
	/// <param name="DeltaTime">time since last frame</param>
	virtual void Tick(float DeltaTime) override;
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
	/// shot rate in seconds
	/// </summary>
	float m_shotRate = 1.0f;

	/// <summary>
	/// timer to shoot
	/// </summary>
	float m_shotTimer = 0.0f;

	/// <summary>
	/// current position on spline
	/// </summary>
	float m_positionOnSpline = 0.0f;
#pragma endregion

#pragma region private variable
	/// <summary>
	/// target location
	/// </summary>
	FVector m_target = FVector();
#pragma endregion

#pragma region private pointer
	/// <summary>
	/// spline reference
	/// </summary>
	USplineComponent* m_pSpline = nullptr;
#pragma endregion
};