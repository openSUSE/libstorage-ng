/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016,2018] SUSE LLC
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
#include <vector>


namespace storage
{

    using namespace std;


    /**
     * Simple class to keep a native and a translated string in
     * parallel. Purpose is to be able to log user messages in English.
     */
    class Text
    {
    public:

	Text() : native(), translated() {}
	Text(const char* native, const char* translated) : native(native), translated(translated) {}
	Text(const string& native, const string& translated) : native(native), translated(translated) {}

	bool empty() { return native.empty(); }

	void clear();

	const Text& operator+=(const Text& a);

	string native;
	string translated;
    };


    /**
     * Untranslated version of class Text. Useful for fatal logical error
     * messages that need no translation but where a Text object is required.
     */
    struct UntranslatedText : public Text
    {
	UntranslatedText(const char* native) : Text(native, native) {}
	UntranslatedText(const string& native) : Text(native, native) {}
    };


    Text _(const char* msgid);
    Text _(const char* msgid, const char* msgid_plural, unsigned long int n);


    /**
     * Join mode for join function.
     */
    enum class JoinMode
    {
	/**
	 * Join values with newlines.
	 */
	NEWLINE,

	/**
	 * Join values with commas.
	 */
	COMMA
    };


    /**
     * See join(const vector<Text>&, JoinMode, size_t).
     */
    Text
    join(const vector<string>& values, JoinMode join_mode, size_t limit);


    /**
     * Joins the values either with newlines or commas depending on
     * the join_mode. The number of values include in the result is
     * limited by limit. If values are omitted the result includes a
     * text like "and 7 more".
     */
    Text
    join(const vector<Text>& values, JoinMode join_mode, size_t limit);


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
