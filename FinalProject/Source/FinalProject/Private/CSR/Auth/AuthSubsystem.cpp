// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/Auth/AuthSubsystem.h"
#include "JsonObjectConverter.h"

void UAuthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    HttpModule = &FHttpModule::Get();
}

void UAuthSubsystem::Deinitialize()
{
    Super::Deinitialize();
    HttpModule = nullptr;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UAuthSubsystem::CreateHttpRequest(const FString& Endpoint)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
    UE_LOG(LogTemp, Warning, TEXT("%s%s"), *this->AuthServerUrl, *Endpoint);
    Request->SetURL(FString::Printf(TEXT("%s%s"), *this->AuthServerUrl, *Endpoint));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    return Request;
}

void UAuthSubsystem::RequestSignUp(const FString& UserID, const FString& Password, const FString& Nickname)
{
    if (!HttpModule)
    {
        OnSignUpResponse.Broadcast(false, TEXT("HTTP Module not initialized"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateHttpRequest(this->AuthServerJoin);
    Request->OnProcessRequestComplete().BindUObject(this, &UAuthSubsystem::OnSignUpRequestComplete);

    // JSON 데이터 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("id"), UserID);
    JsonObject->SetStringField(TEXT("passwd"), Password);
    JsonObject->SetStringField(TEXT("name"), Nickname);

    FString RequestString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *RequestString);

    Request->SetContentAsString(RequestString);
    Request->ProcessRequest();
}

void UAuthSubsystem::RequestLogin(const FString& UserID, const FString& Password)
{
    if (!HttpModule)
    {
        OnLoginResponse.Broadcast(false, TEXT("HTTP Module not initialized"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateHttpRequest(this->AuthServerLogin);
    Request->OnProcessRequestComplete().BindUObject(this, &UAuthSubsystem::OnLoginRequestComplete);

    // JSON 데이터 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField(TEXT("id"), UserID);
    JsonObject->SetStringField(TEXT("passwd"), Password);

    FString RequestString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *RequestString);

    Request->SetContentAsString(RequestString);
    Request->ProcessRequest();
}

void UAuthSubsystem::OnSignUpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        OnSignUpResponse.Broadcast(false, TEXT("Network Error"));
        return;
    }


    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    UE_LOG(LogTemp, Warning, TEXT("%d"), Response->GetResponseCode());
    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        bool bSuccessful = Response->GetResponseCode() == 200;
        FString Message;
        if (JsonObject->HasField("message"))
        {
            // 서버에서 message 필드를 보내줬다면 그 메시지를 사용
            Message = JsonObject->GetStringField("message");
        }
        else
        {
            // 서버에서 message 필드가 없다면 기본 메시지 사용
            if (bSuccessful)
            {
                Message = TEXT("회원가입 성공");
            }
            else
            {
                Message = TEXT("회원가입 실패");
            }
        }

        OnSignUpResponse.Broadcast(bSuccessful, Message);
    }
    else
    {
        OnSignUpResponse.Broadcast(false, TEXT("Invalid Response"));
    }
}

void UAuthSubsystem::OnLoginRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        OnLoginResponse.Broadcast(false, TEXT("Network Error"));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    UE_LOG(LogTemp, Warning, TEXT("%d"), Response->GetResponseCode());
    //UE_LOG(LogTemp, Warning, TEXT("%d"), Response->GetResponseCode());

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        bool bSuccessful = Response->GetResponseCode() == 200;

        // 토큰이 있는지 확인
        if (bSuccessful && JsonObject->HasField("token"))
        {
            AuthToken = JsonObject->GetStringField("token");
        }

        FString Message;
        if (JsonObject->HasField("message"))
        {
            // 서버에서 message 필드를 보내줬다면 그 메시지를 사용
            Message = JsonObject->GetStringField("message");
        }
        else
        {
            // 서버에서 message 필드가 없다면 기본 메시지 사용
            if (bSuccessful)
            {
                Message = TEXT("로그인 성공");
            }
            else
            {
                Message = TEXT("로그인 실패");
            }
        }

        OnLoginResponse.Broadcast(bSuccessful, Message);
    }
    else
    {
        OnLoginResponse.Broadcast(false, TEXT("Invalid Response"));
    }
}