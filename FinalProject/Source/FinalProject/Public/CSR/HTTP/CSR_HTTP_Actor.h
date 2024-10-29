// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "CSR_HTTP_Actor.generated.h"

UCLASS()
class FINALPROJECT_API ACSR_HTTP_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSR_HTTP_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	const FString UMAP_FILE_PATH = FPaths::ProjectContentDir() / TEXT("PSH/PSH_Map/PSH_Level.umap");

	const FString SERVERSAVE_URL = "http://www.mtvs.store:1818/api/v1/map/upload";

	//const FString SERVERSAVE_URL = "http://127.0.0.1:5000/upload";

	const FString SERVERDOWN_URL = "http://www.mtvs.store:1818/api/v1/map/download?no=";

	UPROPERTY(EditAnywhere)
	FString a = "";
	//const FString SERVERDOWN_URL = "http://127.0.0.1:5000/down";

	const FString BOUNDARY = "----WebKitFormBoundary7MA4YWxkTrZu0gW";

	// util 함수
	TArray<uint8> LoadUMapFile(const FString& FilePath);

	// 2. FPSH_ObjectData를 JSON 문자열로 변환하는 함수
	FString ConvertStructToJsonString(const FPSH_ObjectData& ObjectData);

	TArray<uint8> CreateMultipartData(const TArray<uint8>& FileData, const FString& JsonString);

	UFUNCTION(BlueprintCallable)
	void SendUMapAndJsonToServer();

	UFUNCTION()
	void Req_SaveUMapAndJson(const TArray<uint8>& FileData, const FString& JsonString);

	void Res_SaveUMapAndJson(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void Req_DownMap();

	void Res_DownMap(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
