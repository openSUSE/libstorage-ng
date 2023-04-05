/*
 * Copyright (c) 2023 SUSE LLC
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


#include "storage/SystemInfo/CmdNvme.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/JsonFile.h"


namespace storage
{

    CmdNvmeList::CmdNvmeList()
    {
	SystemCmd cmd(NVME_BIN " list --verbose --output json", SystemCmd::DoThrow);

	parse(cmd.stdout());
    }


    void
    CmdNvmeList::parse(const vector<string>& lines)
    {
        JsonFile json_file(lines);
    }

}
