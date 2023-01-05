// Copyright 2021, Dakota Dawe, All rights reserved

#include "Misc/HttpHelper.h"

FAPIRespEvent UHttpHelper::CachedRespEvent = FAPIRespEvent();
FHttpModule* UHttpHelper::HTTP = nullptr;

void UHttpHelper::RESP_HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString RespString = "";

	if (Response) {
		RespString = Response->GetContentAsString();
	}

	if (CachedRespEvent.IsBound()){
		CachedRespEvent.Broadcast(RespString);
	}
}

void UHttpHelper::API_PostRequest(FAPIRespEvent RespEvent, const FString& Handle, const FString& PostData)
{
	if (HTTP)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HTTP->CreateRequest();
		Request->OnProcessRequestComplete().BindStatic(&UHttpHelper::RESP_HandleResponse);

		CachedRespEvent = RespEvent;
		
		FString URL = API_URL;
		URL.Append(Handle);

		Request->SetURL(URL);
		Request->SetVerb("POST");
		Request->SetContentAsString(PostData);
		Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
		Request->SetHeader("Content-Type", TEXT("application/json"));
		Request->ProcessRequest();
	}
	else
	{
		if (GetHttpModule())
		{
			API_GetRequest(RespEvent, Handle, PostData);
		}
	}
}

void UHttpHelper::API_GetRequest(FAPIRespEvent RespEvent, const FString& Handle, const FString& GetData)
{
	if (HTTP)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HTTP->CreateRequest();
		Request->OnProcessRequestComplete().BindStatic(&UHttpHelper::RESP_HandleResponse);

		CachedRespEvent = RespEvent;

		FString URL = API_URL;
		URL.Append(Handle + "/" + GetData);

		Request->SetURL(URL);
		Request->SetVerb("GET");
		//Request->SetContentAsString(GetData);
		Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
		Request->SetHeader("Content-Type", TEXT("application/json"));
		Request->ProcessRequest();
	}
	else
	{
		if (GetHttpModule())
		{
			API_GetRequest(RespEvent, Handle, GetData);
		}
	}
}

FHttpModule* UHttpHelper::GetHttpModule()
{
	if (!HTTP)
	{
		HTTP = &FHttpModule::Get();
	}
	return HTTP;
}
