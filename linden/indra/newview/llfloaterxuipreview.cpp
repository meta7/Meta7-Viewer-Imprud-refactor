/** 
 * @file llfloaterxuipreview.cpp
 * @brief LLFloaterXUIPreview class implementation
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

#include "llfloaterxuipreview.h"

#include "llerror.h"
#include "llstring.h"
#include "llviewertexteditor.h"
#include "lluictrlfactory.h"

//
// Member Functions
//
LLFloaterXUIPreview::LLFloaterXUIPreview()
:	LLFloater(std::string("preview floater"))
{
	mTracedFloater = NULL;
	mTracedFilename = "";
	mInputEditor = NULL;

	LLUICtrlFactory::getInstance()->buildFloater(this,"floater_xui_preview.xml");
}

LLFloaterXUIPreview::~LLFloaterXUIPreview()
{
	// Children all cleaned up by default view destructor.
}

BOOL LLFloaterXUIPreview::postBuild()
{
	childSetAction("send to chat", onClickSend, this);
	childSetAction("reload XML", onClickReload, this);
	childSetVisible("reload XML", FALSE);

	mInputEditor = getChild<LLLineEditor>("chat channel");
	if (mInputEditor)
	{
		mInputEditor->setCallbackUserData(this);
		//mInputEditor->setKeystrokeCallback(&onInputEditorKeystroke);
		mInputEditor->setCommitOnFocusLost( FALSE );
		mInputEditor->setRevertOnEsc( FALSE );
		mInputEditor->setIgnoreTab(TRUE);
		mInputEditor->setPassDelete(TRUE);
		mInputEditor->setReplaceNewlinesWithSpaces(FALSE);

		mInputEditor->setMaxTextLength(10);
		mInputEditor->setEnableLineHistory(TRUE);
	}

	return TRUE;
}

// static
void LLFloaterXUIPreview::onClickReload(void* userdata)
{
	LLFloaterXUIPreview* self = (LLFloaterXUIPreview*)userdata;

	if (self->mTracedFilename != "")
	{
		if(self->mTracedFloater)
		{
			LLUICtrlFactory::getInstance()->removeFloater(self->mTracedFloater);
		}
		else
		{
			self->mTracedFloater = new LLFloaterProxy("sample_floater");
			self->mTracedFloater->setTrace(cbAddTrace, self);
		}

		LLUICtrlFactory::getInstance()->buildFloater(self->mTracedFloater, self->mTracedFilename);
	}
}

// static
void LLFloaterXUIPreview::onClickSend(void* userdata)
{
	LLFloaterXUIPreview* self = (LLFloaterXUIPreview*)userdata;

	if (self->mInputEditor)
	{
		LLWString text = self->mInputEditor->getConvertedText();

		std::string utf8text = wstring_to_utf8str(text);

		if(self->mTracedFloater)
		{
			self->mTracedFloater->setChatChannel(utf8text);
		}
	}

	self->childSetValue("chat channel", LLStringUtil::null);
}

// static
std::string LLFloaterXUIPreview::cbAddTrace(const std::string& msg, void* userdata)
{
	LLFloaterXUIPreview* self = (LLFloaterXUIPreview*)userdata;
	self->addTrace(msg);

	return msg;
}

void LLFloaterXUIPreview::addTrace(const std::string& msg)
{
	LLColor4 color = LLColor4::red;

	LLViewerTextEditor*	trace_editor = getChild<LLViewerTextEditor>("XUI preview display");

	trace_editor->setParseHTML(TRUE);
	trace_editor->setParseHighlights(TRUE);

	trace_editor->appendTime(true);
	trace_editor->appendColoredText(msg, false, false, color);
}

void LLFloaterXUIPreview::setTracedFloater(LLFloaterProxy* traced_floater, std::string filename)
{
	if(!traced_floater)
	{
		// the traced floater is disconnecting from us
		mTracedFloater = NULL;

		return;
	}

	if(mTracedFloater)
	{
		mTracedFloater->setTrace(NULL, NULL);
	}

	mTracedFloater = traced_floater;
	mTracedFilename = filename;

	if (filename == "")
	{
		childSetVisible ("reload XML", FALSE);
	}
	else
	{
		childSetVisible ("reload XML", TRUE);
	}

	traced_floater->setTrace(cbAddTrace, this);
}

// public virtual
void LLFloaterXUIPreview::onClose(bool app_quitting)
{
	if(mTracedFloater)
	{
		mTracedFloater->setTrace(NULL, NULL);
	}

	LLUICtrlFactory::getInstance()->removeFloater(this);

	// destroy ourselves
	LLFloater::onClose (app_quitting);
}

void LLFloaterXUIPreview::setTraceCursorAndScrollToEnd()
{
	LLViewerTextEditor*	trace_editor = getChild<LLViewerTextEditor>("XUI preview display");
	
	if (trace_editor) 
	{
		trace_editor->setCursorAndScrollToEnd();
	}
}
