/** 
 * @file llfloaterxuipreview.h
 * @brief LLFloaterXUIPreview class definition
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


#ifndef LL_LLFLOATERXUIPREVIEW_H
#define LL_LLFLOATERXUIPREVIEW_H

#include "llfloater.h"
#include "lllineeditor.h"
#include "llfloaterproxy.h"

class LLFloaterXUIPreview
	:	public LLFloater
{
public:
	LLFloaterXUIPreview();
	~LLFloaterXUIPreview();

	virtual BOOL postBuild();
	virtual void onClose(bool app_quitting);

	static std::string cbAddTrace(const std::string& msg, void* userdata);
	void addTrace(const std::string& msg);

	void setTracedFloater(LLFloaterProxy* traced_floater, std::string filename = "");

	static void onClickReload(void* userdata);
	static void onClickSend(void* userdata);

	void setTraceCursorAndScrollToEnd();
	
private:
	BOOL mScrolledToEnd;
	LLFloaterProxy* mTracedFloater;
	std::string     mTracedFilename;
	LLLineEditor* mInputEditor;
};

#endif
