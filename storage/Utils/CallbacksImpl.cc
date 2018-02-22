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


#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Text.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"


namespace storage
{

    void
    message_callback(const Callbacks* callbacks, const Text& message)
    {
	if (callbacks)
	{
	    callbacks->message(message.translated);
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

}
