// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoseArm.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AUIRoseArm::AUIRoseArm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));


    // Create and attach arm skeletal mesh component
    ArmMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMeshComponent"));
    ArmMeshComponent->SetupAttachment(RootComponent);
    ArmMeshComponent->SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f)); // Adjust location if needed
    ArmMeshComponent->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmMeshAsset(TEXT("/Game/UI/StartLevel/RoseArm/RoseArm_SkeletalMesh.RoseArm_SkeletalMesh"));
    if (ArmMeshAsset.Succeeded())
    {
        ArmMeshComponent->SetSkeletalMesh(ArmMeshAsset.Object);
    }

    // Create and attach cody skeletal mesh component
    CodyMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CodyMeshComponent"));
    CodyMeshComponent->SetupAttachment(RootComponent);
    CodyMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Adjust location if needed
    CodyMeshComponent->SetWorldScale3D(FVector(5.0f, 5.0f, 5.0f)); // 코디 메시의 스케일을 (2, 2, 2)로 설정합니다.

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> CodyMeshAsset(TEXT("/Game/UI/StartLevel/CodyDoll/CodyDoll_SkeletalMesh.CodyDoll_SkeletalMesh"));
    if (CodyMeshAsset.Succeeded())
    {
        CodyMeshComponent->SetSkeletalMesh(CodyMeshAsset.Object);
    }

    // Create and attach may skeletal mesh component
    MayMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MayMeshComponent"));
    MayMeshComponent->SetupAttachment(RootComponent);
    MayMeshComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f)); // Adjust location if needed
    MayMeshComponent->SetWorldScale3D(FVector(5.0f, 5.0f, 5.0f)); // 코디 메시의 스케일을 (2, 2, 2)로 설정합니다.

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MayMeshAsset(TEXT("/Game/UI/StartLevel/MayDoll/MayDoll_SkeletalMesh.MayDoll_SkeletalMesh"));
    if (MayMeshAsset.Succeeded())
    {
        MayMeshComponent->SetSkeletalMesh(MayMeshAsset.Object);
    }


    // /Script/Engine.SkeletalMesh'/Game/UI/StartLevel/RoseArm/RoseArm_SkeletalMesh.RoseArm_SkeletalMesh'
    ///Script/Engine.SkeletalMesh'/Game/UI/StartLevel/CodyDoll/CodyDoll_SkeletalMesh.CodyDoll_SkeletalMesh'
    ///Script/Engine.SkeletalMesh'/Game/UI/StartLevel/MayDoll/MayDoll_SkeletalMesh.MayDoll_SkeletalMesh'
	
}

// Called when the game starts or when spawned
void AUIRoseArm::BeginPlay()
{
	Super::BeginPlay();



	//Attach MayMesh to LeftAttach bone
	MayMeshComponent->AttachToComponent(ArmMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("LeftAttach"));
    MayMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Adjust location if needed
    MayMeshComponent->SetRelativeRotation_Direct(FRotator(20.0f, 0.0f, 80.0f)); // Adjust location if needed

	//Attach CodyMesh to RightAttach bone
	CodyMeshComponent->AttachToComponent(ArmMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("RightAttach"));
    CodyMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Adjust location if needed
    CodyMeshComponent->SetRelativeRotation_Direct(FRotator(140.0f, 0.0f, -110.0f)); // Adjust location if needed


    //FTransform SocketTransform = ArmMeshComponent->GetSocketTransform(FName("LeftAttach"));

    //// Set the transform of MayMeshComponent to match the socket's transform
    //MayMeshComponent->SetWorldLocation(SocketTransform.GetLocation());
    ////MayMeshComponent->SetWorldRotation(SocketTransform.GetRotation());

    //// You may need to adjust the location and rotation based on the specific offset required for May's mesh
    //// For example, you can apply additional offset if necessary:;

    //// Repeat the same process for CodyMeshComponent
    //SocketTransform = ArmMeshComponent->GetSocketTransform(FName("RightAttach"));
    //CodyMeshComponent->SetWorldLocation(SocketTransform.GetLocation());
    ////CodyMeshComponent->SetWorldRotation(SocketTransform.GetRotation());

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	//D: / project / UE5_Ittakestwo / Content / UI / StartLevel / RoseArm / RoseArm_SkeletalMesh.uasset
}

// Called every frame
void AUIRoseArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

