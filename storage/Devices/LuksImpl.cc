/*
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


#include <iostream>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Luks>::classname = "Luks";


    Luks::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::save(xmlNode* node) const
    {
	Encryption::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::probe_lukses(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdDmsetupInfo::value_type& value : systeminfo.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem != "CRYPT")
		continue;

	    const CmdCryptsetup& cmd_cryptsetup = systeminfo.getCmdCryptsetup(value.first);
	    if (cmd_cryptsetup.encryption_type != EncryptionType::LUKS)
		continue;

	    Luks* luks = Luks::create(probed, value.first);
	    luks->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    Luks::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_1(probed, systeminfo);

	const File size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get_int(), 512));
    }


    void
    Luks::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_2(probed, systeminfo);

	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid = systeminfo.getBlkid();
	Blkid::Entry entry2;
	if (!blkid.find_by_name(blk_device->get_name(), entry2, systeminfo))
	    ST_THROW(Exception("failed to probe luks uuid"));

	uuid = entry2.luks_uuid;
    }


    void
    Luks::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    Luks::Impl::calculate_region()
    {
	const BlkDevice* blk_device = get_blk_device();

	unsigned long long size = blk_device->get_size();

	// size of luks metadata is explained at
	// https://gitlab.com/cryptsetup/cryptsetup/wikis/FrequentlyAskedQuestions

	if (size > 2 * MiB)
	    size -= 2 * MiB;
	else
	    size = 0 * B;

	set_size(size);
    }


    bool
    Luks::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Encryption::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    Luks::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Encryption::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    Luks::Impl::print(std::ostream& out) const
    {
	Encryption::Impl::print(out);

	out << " uuid:" << uuid;
    }


    void
    Luks::Impl::do_create() const
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode luksFormat " + quote(get_blk_device()->get_name()) +
	    " --key-file -";
	cout << cmd_line << endl;

	SystemCmd cmd;
	cmd.setStdinText(get_password());
	cmd.execute(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create Luks failed"));
    }


    void
    Luks::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUPBIN " --batch-mode erase " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete Luks failed"));

	// cryptsetup erase does not remove the signature, thus also use
	// generic wipefs.

	blk_device->get_impl().wipe_device();
    }


    void
    Luks::Impl::do_activate() const
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode luksOpen " + quote(get_blk_device()->get_name()) + " " +
	    quote(get_dm_table_name()) + " --key-file -";
	cout << cmd_line << endl;

	SystemCmd cmd;
	cmd.setStdinText(get_password());
	cmd.execute(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("activate Luks failed"));
    }


    void
    Luks::Impl::do_deactivate() const
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode close " + quote(get_dm_table_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("deactivate Luks failed"));
    }

}
