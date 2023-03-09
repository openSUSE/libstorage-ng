/*
 * Copyright (c) [2018-2023] SUSE LLC
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


#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Text.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Storage.h"


namespace storage
{

    CallbacksGuard::CallbacksGuard(const Callbacks* callbacks)
	: callbacks(callbacks)
    {
	const ProbeCallbacksV3* probe_callbacks_v3 = dynamic_cast<const ProbeCallbacksV3*>(callbacks);
	if (probe_callbacks_v3)
	    probe_callbacks_v3->begin();

	const ActivateCallbacksV3* activate_callbacks_v3 = dynamic_cast<const ActivateCallbacksV3*>(callbacks);
	if (activate_callbacks_v3)
	    activate_callbacks_v3->begin();

	const CommitCallbacksV2* commit_callbacks_v2 = dynamic_cast<const CommitCallbacksV2*>(callbacks);
	if (commit_callbacks_v2)
	    commit_callbacks_v2->begin();
    }


    CallbacksGuard::~CallbacksGuard()
    {
	const ProbeCallbacksV3* probe_callbacks_v3 = dynamic_cast<const ProbeCallbacksV3*>(callbacks);
	if (probe_callbacks_v3)
	    probe_callbacks_v3->end();

	const ActivateCallbacksV3* activate_callbacks_v3 = dynamic_cast<const ActivateCallbacksV3*>(callbacks);
	if (activate_callbacks_v3)
	    activate_callbacks_v3->end();

	const CommitCallbacksV2* commit_callbacks_v2 = dynamic_cast<const CommitCallbacksV2*>(callbacks);
	if (commit_callbacks_v2)
	    commit_callbacks_v2->end();
    }


    ActionCallbacksGuard::ActionCallbacksGuard(const CommitCallbacks* commit_callbacks, const Action::Base* action)
	: commit_callbacks(commit_callbacks), action(action)
    {
	const CommitCallbacksV2* commit_callbacks_v2 = dynamic_cast<const CommitCallbacksV2*>(commit_callbacks);
	if (commit_callbacks_v2)
	    commit_callbacks_v2->begin_action(action);
    }


    ActionCallbacksGuard::~ActionCallbacksGuard()
    {
	const CommitCallbacksV2* commit_callbacks_v2 = dynamic_cast<const CommitCallbacksV2*>(commit_callbacks);
	if (commit_callbacks_v2)
	    commit_callbacks_v2->end_action(action);
    }


    void
    message_callback(const Callbacks* callbacks, const Text& message)
    {
	if (callbacks)
	{
	    callbacks->message(message.translated);
	}
    }


    void
    error_callback(const Callbacks* callbacks, const Text& message)
    {
	if (callbacks)
	{
	    if (!callbacks->error(message.translated, ""))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    ST_THROW(Exception(message.native));
	}
    }


    void
    ambiguity_partition_table_and_filesystem_callback(const ProbeCallbacks* probe_callbacks, const Text& message,
						      const std::string& name, PtType pt_type, FsType fs_type)
    {
	const ProbeCallbacksV4* probe_callbacks_v4 = dynamic_cast<const ProbeCallbacksV4*>(probe_callbacks);

	if (probe_callbacks_v4)
	{
	    if (!probe_callbacks_v4->ambiguity_partition_table_and_filesystem(message.translated, "", name,
									      pt_type, fs_type))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    error_callback(probe_callbacks, message);
	}
    }


    void
    ambiguity_partition_table_and_luks_callback(const ProbeCallbacks* probe_callbacks, const Text& message,
						const std::string& name, PtType pt_type)
    {
	const ProbeCallbacksV4* probe_callbacks_v4 = dynamic_cast<const ProbeCallbacksV4*>(probe_callbacks);

	if (probe_callbacks_v4)
	{
	    if (!probe_callbacks_v4->ambiguity_partition_table_and_luks(message.translated, "", name, pt_type))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    error_callback(probe_callbacks, message);
	}
    }


    void
    ambiguity_partition_table_and_lvm_pv_callback(const ProbeCallbacks* probe_callbacks, const Text& message,
						  const std::string& name, PtType pt_type)
    {
	const ProbeCallbacksV4* probe_callbacks_v4 = dynamic_cast<const ProbeCallbacksV4*>(probe_callbacks);

	if (probe_callbacks_v4)
	{
	    if (!probe_callbacks_v4->ambiguity_partition_table_and_lvm_pv(message.translated, "", name, pt_type))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    error_callback(probe_callbacks, message);
	}
    }


    void
    unsupported_partition_table_callback(const ProbeCallbacks* probe_callbacks, const Text& message,
					 const std::string& name, PtType pt_type)
    {
	const ProbeCallbacksV4* probe_callbacks_v4 = dynamic_cast<const ProbeCallbacksV4*>(probe_callbacks);

	if (probe_callbacks_v4)
	{
	    if (!probe_callbacks_v4->unsupported_partition_table(message.translated, "", name, pt_type))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    error_callback(probe_callbacks, message);
	}
    }


    void
    unsupported_filesystem_callback(const ProbeCallbacks* probe_callbacks, const Text& message,
				    const std::string& name, FsType fs_type)
    {
	const ProbeCallbacksV4* probe_callbacks_v4 = dynamic_cast<const ProbeCallbacksV4*>(probe_callbacks);

	if (probe_callbacks_v4)
	{
	    if (!probe_callbacks_v4->unsupported_filesystem(message.translated, "", name, fs_type))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    error_callback(probe_callbacks, message);
	}
    }


    void
    error_callback(const Callbacks* callbacks, const Text& message, const Exception& exception)
    {
	ST_CAUGHT(exception);

	if (typeid(exception) == typeid(Aborted))
	    ST_RETHROW(exception);

	if (callbacks)
	{
	    if (!callbacks->error(message.translated, exception.what()))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after error");
	}
	else
	{
	    ST_RETHROW(exception);
	}
    }


    void
    missing_command_callback(const ProbeCallbacksV2* probe_callbacks, const Text& message, const std::string& command,
			     uint64_t features, const Exception& exception)
    {
	ST_CAUGHT(exception);

	if (typeid(exception) == typeid(Aborted))
	    ST_RETHROW(exception);

	if (probe_callbacks)
	{
	    if (!probe_callbacks->missing_command(message.translated, exception.what(), command, features))
		ST_THROW(Aborted("aborted"));

	    y2mil("user decides to continue after missing command");
	}
	else
	{
	    ST_RETHROW(exception);
	}
    }

}
