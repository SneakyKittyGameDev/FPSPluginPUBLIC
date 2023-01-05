// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/PDA_AttachmentCompatibility.h"

UPDA_AttachmentCompatibility::UPDA_AttachmentCompatibility()
{
	
}

bool UPDA_AttachmentCompatibility::EnableAttachment(FSKGDataAssetAttachment& Attachment, const bool bEnable)
{
	const int32 Index = Attachments.Find(Attachment);
	if (Index != INDEX_NONE)
	{
		Attachments[Index].bEnabledForUse = bEnable;
		return true;
	}
	return false;
}
