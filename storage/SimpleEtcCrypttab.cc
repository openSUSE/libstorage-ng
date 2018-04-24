/*
 * Copyright (c) 2018 SUSE LLC
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


#include "storage/SimpleEtcCrypttab.h"
#include "storage/EtcCrypttab.h"


namespace storage
{

    vector<SimpleEtcCrypttabEntry>
    read_simple_etc_crypttab(const string& filename)
    {
	EtcCrypttab etc_crypttab(filename);

	vector<SimpleEtcCrypttabEntry> ret;

	for (int i = 0; i < etc_crypttab.get_entry_count(); ++i)
	{
	    const CrypttabEntry* crypttab_entry = etc_crypttab.get_entry(i);

	    SimpleEtcCrypttabEntry tmp;

	    tmp.name = crypttab_entry->get_crypt_device();
	    tmp.device = crypttab_entry->get_block_device();
	    tmp.password = crypttab_entry->get_password();
	    tmp.crypt_options = crypttab_entry->get_crypt_opts().get_opts();

	    ret.push_back(tmp);
	}

	return ret;
    }

}
