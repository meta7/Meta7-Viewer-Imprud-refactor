/** 
 * @file llfloaterproxy.cpp
 * @brief LLFloaterProxy class implementation
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2002-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llfloaterproxy.h"
#include "llfloaterxuipreview.h"
#include "llchatbar.h"

#include "llerror.h"
#include "llstring.h"
#include "lluictrlfactory.h"

//
// Member Functions
//
LLFloaterProxy::LLFloaterProxy(const std::string& name)
:	LLFloater(name)
{
	mTracingFloater = NULL;
	mTracingCallback = NULL;
	mSendToChat = FALSE;
	mChatChannel = "";
}

LLFloaterProxy::~LLFloaterProxy()
{
	// Children all cleaned up by default view destructor.
}

BOOL LLFloaterProxy::postBuild()
{
	LLFloater::setTrace(cbAddTrace, this);

	return TRUE;
}

BOOL LLFloaterProxy::setChatChannel(std::string channel)
{
	if(channel == "")
	{
		mSendToChat = FALSE;
	}
	else
	{
		// need to validate integer
		mSendToChat = TRUE;
		mChatChannel = channel;
	}

	return TRUE;
}

// static
std::string LLFloaterProxy::cbAddTrace(LLView::trace_info& info, void* userdata)
{
	LLFloaterProxy* self = (LLFloaterProxy*)userdata;
	self->addTrace(info);

	if(self->mSendToChat)
	{
		std::string message = LLURI::escape(*info.mName) + " " + (*info.mAction);

		if (info.mResult)
		{
			int header_length = message.length();
			int result_length = info.mResult->length();

			if (header_length + result_length < MAX_MSG_STR_LEN)
			{
				message += " " + (*info.mResult);
			}
			else
			{
				// Handle long results here.  The extra 14 is for the action
				// modifier, the sequence number, and two spaces.
				int block_length = MAX_MSG_STR_LEN - (header_length + 14);

				// If the header alone is too long, just let it be truncated and
				// ignore the result.  We don't want to send too many messages.
				if (block_length < 100)
				{
					// Modify the action name so the receiver knows.
					// It might get truncated, but we do our best.
					message += "_toolong";
				}
				else
				{
					// Modify the action name so the receiver knows.
					message += "_long";

					int sections = result_length / block_length;
					int last_section = result_length - (sections * block_length);
					if (last_section == 0)
					{
						last_section = block_length;
						sections--;
					}

					// Send all but the last section
					int start = 0;
					for ( ; sections > 0; sections-- )
					{
						std::ostringstream remaining;
						remaining << sections;
						std::string partial = remaining.str() + " " + info.mResult->substr(start, block_length);
						std::string section = "/" + self->mChatChannel + " " + message + " " + partial; 

						gChatBar->sendChatFromViewer(section, CHAT_TYPE_NORMAL, FALSE);

						start += block_length;
					}

					message += " 0 " + info.mResult->substr(start, last_section);
				}
			}
		}

		message = utf8str_truncate (message, MAX_MSG_STR_LEN);
		message = "/" + self->mChatChannel + " " + message;

		gChatBar->sendChatFromViewer(message, CHAT_TYPE_NORMAL, FALSE);

		return self->mChatChannel;
	}
	else
	{
		return "";
	}
}

void LLFloaterProxy::addTrace(LLView::trace_info& info)
{
	if(mTracingFloater)
	{
		// ignoring the return string because it's empty
		(*mTracingCallback)(info, mTracingFloater);
	}
}

void LLFloaterProxy::setTrace( std::string (*callback)(LLView::trace_info&, void*), void* userdata)
{
	if(mTracingFloater)
	{
		((LLFloaterXUIPreview*) mTracingFloater)->setTracedFloater(NULL);
	}

	mTracingCallback = callback;
	mTracingFloater = userdata;
}

// public virtual
void LLFloaterProxy::onClose(bool app_quitting)
{
	if(mTracingFloater)
	{
		((LLFloaterXUIPreview*) mTracingFloater)->setTracedFloater(NULL);
	}

	// destroy ourselves
	LLFloater::onClose (app_quitting);
}
