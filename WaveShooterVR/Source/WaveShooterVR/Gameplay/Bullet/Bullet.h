#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h" 
#pragma endregion

#pragma region project include
#include "Bullet.generated.h"
#pragma endregion

UCLASS()
/// <summary>
/// bullet class
/// </summary>
class WAVESHOOTERVR_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	///	</summary>
	ABullet();
#pragma endregion

#pragma region public override function
	/// <summary>
	/// update every frame
	/// </summary>
	/// <param name="DeltaTime">time since last frame</param>
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Movement")
	/// <summary>
	/// static mesh
	/// </summary>
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	/// <summary>
	/// movement speed in cm per second
	/// </summary>
	float MovementSpeed = 1000.0f;
#pragma endregion

#pragma region UFUNCTION
	UFUNCTION(BlueprintCallable, Category = "Bullet")
	/// <summary>
	/// collide with other actor
	/// </summary>
	/// <param name="OtherActor">other actor</param>
	void Collide(AActor* OtherActor);
#pragma endregion
};