/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_TEXT_H
#define STORAGE_TEXT_H


#include <string>


namespace storage
{

    using namespace std;


    /**
     * Simple class to keep a native and a translated string in
     * parallel. Purpose is to be able to log user messages in English.
     */
    struct Text
    {
	Text() : native(), text() {}
	Text(const string& native, const string& text) : native(native), text(text) {}

	void clear();

	const Text& operator+=(const Text& a);

	string native;
	string text;
    };


    /**
     * Untranslated version of class Text. Useful for fatal logical error
     * messages that need no translation but where a Text object is required.
     */
    struct UntranslatedText : public Text
    {
	UntranslatedText(const string& native) : Text(native, native) {}
    };


    Text sformat(const Text& format, ...);


    Text _(const char* msgid);
    Text _(const char* msgid, const char* msgid_plural, unsigned long int n);


    enum class Tense
    {
	SIMPLE_PRESENT, PRESENT_CONTINUOUS
    };


    /**
     * Function to select between tenses. Implemented as macro to avoid all
     * arguments to be evaluated.
     */
#define tenser(tense, msgid_simple_present, msgid_present_continuous)  \
    (tense == Tense::SIMPLE_PRESENT ? (msgid_simple_present) : (msgid_present_continuous))

}


#endif
