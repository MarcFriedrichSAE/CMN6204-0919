#pragma region project include
#include "PlayerPawn.h"
#include "Gameplay/Bullet/Bullet.h"
#pragma endregion

#pragma region UE4 include
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#pragma endregion

#pragma region constructor
// constructor
APlayerPawn::APlayerPawn()
{
	// enable update
	PrimaryActorTick.bCanEverTick = true;
	
	// create default scene component and make root
	USceneComponent* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	// create default capsule component and attach to root
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(pRoot);

	// create default scene component and attach to capsule
	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Capsule);

	// create default skeletal mesh component and attach to capsule
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Capsule);

	// create default scene component and attach to capsule
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(Capsule);

	// create default camera component and attach to camera root
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(CameraRoot);
}
#pragma endregion

#pragma region public override function
// update every frame
void APlayerPawn::Tick(float DeltaTime)
{
	// parent update
	Super::Tick(DeltaTime);
}
#pragma endregion

#pragma region UFUNCTION
// move player
void APlayerPawn::Move(float ForwardBack, float LeftRight)
{
	// calculate direction to move to
	FVector dir = Capsule->GetForwardVector() * ForwardBack;
	dir += Capsule->GetRightVector() * LeftRight;
	dir *= GetWorld()->GetDeltaSeconds() * MovementSpeed;

	// try to add world offset with collision detection
	Capsule->AddWorldOffset(dir, true);
}

// rotate player and camera
void APlayerPawn::Rotate(float LeftRight, float UpDown)
{
	// calculate yaw rotation and add to capsule
	FRotator rotation = FRotator(0.0f, LeftRight * GetWorld()->GetDeltaSeconds() * RotationSpeed, 0.0f);
	Capsule->AddLocalRotation(rotation);

	// calculate pitch rotation
	rotation = FRotator(UpDown * GetWorld()->GetDeltaSeconds() * RotationSpeed, 0.0f, 0.0f);

	// if pitch rotation is out of bounds return
	if (CameraRoot->RelativeRotation.Pitch + rotation.Pitch > 45.0f || CameraRoot->RelativeRotation.Pitch + rotation.Pitch < -45.0f)
		return;

	// add pitch rotation
	CameraRoot->AddLocalRotation(rotation);
}

// shoot
void APlayerPawn::Shoot()
{
	// spawn bullet at spawn point
	AActor* pBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, 
		SpawnPoint->GetComponentLocation(), CameraRoot->GetComponentRotation());
}
#pragma endregion

#pragma region protected override function
// called at begin play
void APlayerPawn::BeginPlay()
{
	// parent begin play
	Super::BeginPlay();
}
#pragma endregion