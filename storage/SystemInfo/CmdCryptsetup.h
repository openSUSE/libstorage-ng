/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2019-2022] SUSE LLC
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


#ifndef STORAGE_CMD_CRYPTSETUP_H
#define STORAGE_CMD_CRYPTSETUP_H


#include <string>
#include <vector>

#include "storage/Devices/Encryption.h"


namespace storage
{
    using std::string;
    using std::vector;


    class CmdCryptsetupStatus
    {

    public:

	CmdCryptsetupStatus(const string& name);

	friend std::ostream& operator<<(std::ostream& s, const CmdCryptsetupStatus& cmd_cryptsetup_status);

	EncryptionType get_encryption_type() const { return encryption_type; }

    private:

	void parse(const vector<string>& lines);

	const string name;

	EncryptionType encryption_type;

    };


    class CmdCryptsetupLuksDump
    {

    public:

	CmdCryptsetupLuksDump(const string& name);

	friend std::ostream& operator<<(std::ostream& s, const CmdCryptsetupLuksDump& cmd_cryptsetup_luks_dump);

	const string& get_uuid() const { return uuid; }

	EncryptionType get_encryption_type() const { return encryption_type; }
	const string& get_cipher() const { return cipher; }
	unsigned int get_key_size() const { return key_size; }
	const string& get_pbkdf() const { return pbkdf; }
	const string& get_integrity() const { return integrity; }

    private:

	void parse(const vector<string>& lines);

	void parse_version1(const vector<string>& lines);
	void parse_version2(const vector<string>& lines);

	string name;

	string uuid;

	/**
	 * Either UNKNOWN, LUKS1 or LUKS2
	 */
	EncryptionType encryption_type = EncryptionType::UNKNOWN;

	/**
	 * The cipher, e.g. aes-xts-plain64, twofish-cbc-plain or aes-cbc-plain:sha512.
	 */
	string cipher;

	/**
	 * The size of the master key, e.g. 32 or 64 bytes. Note:
	 * Usually displayed in bits.
	 */
	unsigned int key_size = 0;

	/**
	 * The PBKDF, e.g. argon2i or argon2id, of the first used slot. Only for LUKS2.
	 */
	string pbkdf;

	/**
	 * Is integrity used. Currently is only expected AEAD. Only for LUKS2.
	 */
	string integrity;

    };


    class CmdCryptsetupBitlkDump
    {

    public:

	CmdCryptsetupBitlkDump(const string& name);

	friend std::ostream& operator<<(std::ostream& s, const CmdCryptsetupBitlkDump& cmd_cryptsetup_bitlk_dump);

	const string& get_uuid() const { return uuid; }

	const string& get_cipher() const { return cipher; }
	unsigned int get_key_size() const { return key_size; }

    private:

	void parse(const vector<string>& lines);

	string name;

	string uuid;

	/**
	 * The cipher, e.g. aes-xts-plain64.
	 */
	string cipher;

	/**
	 * The size of the master key, e.g. 32 bytes. Note: Usually displayed in bits.
	 */
	unsigned int key_size = 0;

    };

}

#endif
