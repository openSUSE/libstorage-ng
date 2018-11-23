/*
 * Copyright (c) 2017 SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include "storage/CompoundAction/Formatter.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using std::string;

    CompoundAction::Formatter::Formatter(const CompoundAction::Impl* compound_action,
                                         const string & device_classname)
    : _compound_action(compound_action)
    , _device_classname(device_classname)
    {
        _creating   = has_create(_device_classname);
        _deleting   = has_delete(_device_classname);
        _formatting = has_create<storage::BlkFilesystem>();
        _encrypting = has_create<storage::Encryption>();
        _mounting   = has_create<storage::MountPoint>();
    }


    CompoundAction::Formatter::~Formatter() {}


    string
    CompoundAction::Formatter::string_representation() const
    {
	return text().translated;
    }


    bool
    CompoundAction::Formatter::has_create(const string &device_classname) const
    {
        if (device_classname.empty())
            return false;

        for (const Action::Base * action : _compound_action->get_commit_actions())
        {
            const Action::Create * create_action = dynamic_cast<const Action::Create *>(action);

            if (create_action)
            {
                const Device * device = create_action->get_device(_compound_action->get_actiongraph()->get_impl());

                if (device &&  device->get_impl().get_classname() == device_classname)
                    return true;
            }
        }

        return false;
    }


    bool
    CompoundAction::Formatter::has_delete(const string &device_classname) const
    {
        if (device_classname.empty())
            return false;

        for (const Action::Base * action : _compound_action->get_commit_actions())
        {
            const Action::Delete * delete_action = dynamic_cast<const Action::Delete *>(action);

            if (delete_action)
            {
                Device * device = delete_action->get_device(_compound_action->get_actiongraph()->get_impl());

                if (device &&  device->get_impl().get_classname() == device_classname)
                    return true;
            }
        }

        return false;
    }


    const BlkFilesystem*
    CompoundAction::Formatter::get_created_filesystem() const
    {
	auto action = get_create<storage::BlkFilesystem>();

	if (action)
	{
	    auto device = CompoundAction::Impl::device(_compound_action->get_actiongraph(), action);
	    return to_blk_filesystem(device);
	}

	return nullptr;
    }


    const MountPoint*
    CompoundAction::Formatter::get_created_mount_point() const
    {
	auto action = get_create<storage::MountPoint>();

	if (action)
	{
	    auto device = CompoundAction::Impl::device(_compound_action->get_actiongraph(), action);
	    return to_mount_point(device);
	}

	return nullptr;
    }


    Text
    CompoundAction::Formatter::default_text() const
    {
	const CommitData commit_data(_compound_action->get_actiongraph()->get_impl(), Tense::SIMPLE_PRESENT);
	auto first_action = _compound_action->get_commit_actions().front();
	return first_action->text(commit_data);
    }


    Text
    CompoundAction::Formatter::format_devices_text(const std::vector<const BlkDevice *> &devices)
    {
	std::vector<const BlkDevice *> sorted_devices = devices;
	std::sort( sorted_devices.begin(), sorted_devices.end(),
                   BlkDevice::compare_by_name );

	Text text;

	for ( const BlkDevice * device: sorted_devices )
	{
	    if ( ! text.translated.empty() )
		text += Text( ", ", ", " );

	    // TRANSLATORS:
	    // %1$s is replaced with the device name (e.g. /dev/sdc1),
	    // %2$s is replaced with the size (e.g. 60 GiB)
	    Text dev_text = _( "%1$s (%2$s)" );

	    text += sformat(dev_text, device->get_name(), device->get_size_string());
	}

	return text;
    }

}
