// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/EnemyFreddy.h"
#include "HJS/FreddyPlayer.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEnemyFreddy::AEnemyFreddy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AEnemyFreddy::BeginPlay()
{
	Super::BeginPlay();
	
	SetAILevel(3);

	// ť��(Freddy) ���� Ÿ�̸�
	GetWorld()->GetTimerManager().SetTimer(CubeSpawnTimerHandle, this, &AEnemyFreddy::AttemptSpawnCube, 3.02f, true);

	// Player ��ü ã��
	for (TActorIterator<AFreddyPlayer> It(GetWorld()); It; ++It)
	{
		Player = *It;
		break;
	}
}

// Called every frame
void AEnemyFreddy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPlayerLookingAtBedAndFlashOn() && !GetWorld()->GetTimerManager().IsTimerActive(ShrinkTimerHandle))
	{
		// �����̰� ���� �� ť�갡 �۾�����
		GetWorld()->GetTimerManager().SetTimer(ShrinkTimerHandle, this, &AEnemyFreddy::StartShrinkingCubes, SHRINK_DELAY, false);  
	}
}

void AEnemyFreddy::AttemptSpawnCube()
{
	// ���� �÷��̾ ħ�븦 �ٶ󺸰� ���� �ʴٸ�
	if (!IsPlayerLookingAtBedAndFlashOn())
	{
		// ���� �ѹ��� ����
		int32 RandomNumber = GetRandomNumber();

		// ���� �ѹ��� Freddy�� �������� ���ų� ���ٸ�
		if (RandomNumber <= Level)
		{
			// ť��(Freddy) ���� ����
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FVector SpawnLocation = GetActorLocation();
			switch (SpawnedCubes.Num())
			{
			// Middle Freddy
			case 0: 
				SpawnLocation += FVector(-60, 4580, 530);
				break;
			// Right Freddy
			case 1:
				SpawnLocation += FVector(-530, 4580, 530);
				break;
			// Left Freddy
			case 2:
				SpawnLocation += FVector(420, 4580, 530);
				break;
			}

			FRotator SpawnRotation = FRotator::ZeroRotator;
			AActor* SpawnedCube = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

			if (SpawnedCube)
			{
				UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(SpawnedCube);
				MeshComp->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Script/Engine.SkeletalMesh'/Engine/EngineMeshes/SkeletalCube.SkeletalCube'")).Object);
				MeshComp->SetupAttachment(SpawnedCube->GetRootComponent());
				SpawnedCube->SetRootComponent(MeshComp);
				SpawnedCubes.Add(SpawnedCube);
			}
		}
	}
}

void AEnemyFreddy::ShrinkAndRemoveCubes()
{
	// ť�� ���� ����
	if (ShrinkIndex < SpawnedCubes.Num())
	{
		AActor* CubeToShrink = nullptr;
		switch (SpawnedCubes.Num())
		{
			case 3:
				CubeToShrink = (ShrinkIndex == 0) ? SpawnedCubes[2] : (ShrinkIndex == 1) ? SpawnedCubes[1] :SpawnedCubes[0];
				break;
			case 2:
				CubeToShrink = (ShrinkIndex == 0) ? SpawnedCubes[1] : SpawnedCubes[0];
				break;
			case 1:
				CubeToShrink = SpawnedCubes[0];
				break;
		}
		if (CubeToShrink)
		{
			UStaticMeshComponent* MeshComp = CubeToShrink->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComp)
			{
				FVector NewScale = MeshComp->GetComponentScale() * 0.9f;
				MeshComp->SetWorldScale3D(NewScale);

				// ���� ť���� Scale�� 0.1f ���϶�� Destroy
				if (NewScale.GetMin() < 0.1f)
				{
					SpawnedCubes.Remove(CubeToShrink);
					CubeToShrink->Destroy();
					ShrinkIndex++;
					// 0.5�ʸ��� ť�� remove ����
					GetWorld()->GetTimerManager().SetTimer(ShrinkTimerHandle, this, &AEnemyFreddy::ShrinkAndRemoveCubes, 0.5f, false);
				}
			}
		}
	}
	else
	{
		// ��� ������ ���� �� Index �ʱ�ȭ
		ShrinkIndex = 0;
		GetWorld()->GetTimerManager().ClearTimer(ShrinkTimerHandle);
	}
}

int32 AEnemyFreddy::GetRandomNumber()
{
	return FMath::RandRange(1,20);
}

bool AEnemyFreddy::IsPlayerLookingAtBedAndFlashOn()
{
	if (Player)
	{
		return Player->GetLookAtState() == AFreddyPlayer::LookAt::Bed && Player->GetFlash();
	}
	return false;
}

void AEnemyFreddy::StartShrinkingCubes()
{
	ShrinkIndex = 0;
	ShrinkAndRemoveCubes();
}

