/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#include "storage/SystemInfo/Arch.h"
#include "storage/Utils/AsciiFile.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    // probing must be avoided for the Arch object in StorageImpl
    // TODO overall not nice

    Arch::Arch(bool do_probe)
	: arch("unknown"), ppc_mac(false), ppc_pegasos(false), efiboot(false)
    {
	if (do_probe)
	    probe();
    }


    void
    Arch::readData(const xmlNode* node)
    {
	getChildValue(node, "arch", arch);

	if (!getChildValue(node, "efiboot", efiboot))
	    efiboot = false;
    }


    void
    Arch::saveData(xmlNode* node) const
    {
	setChildValue(node, "arch", arch);

	setChildValueIf(node, "efiboot", efiboot, efiboot);
    }


    void
    Arch::probe()
    {
	SystemCmd cmd(UNAMEBIN " -m");
	if (cmd.retcode() == 0 && cmd.stdout().size() == 1)
	    arch = cmd.stdout().front();

	if (is_ppc())
	{
	    AsciiFile cpuinfo("/proc/cpuinfo");
	    vector<std::string>::const_iterator it = find_if(cpuinfo.lines(), string_starts_with("machine\t"));
	    if (it != cpuinfo.lines().end())
	    {
		y2mil("line:" << *it);

		std::string tmp1 = extractNthWord(2, *it);
		y2mil("tmp1:" << tmp1);
		ppc_mac = boost::starts_with(tmp1, "PowerMac") || boost::starts_with(tmp1, "PowerBook");
		ppc_pegasos = boost::starts_with(tmp1, "EFIKA5K2");

		if (!ppc_mac && !ppc_pegasos)
		{
		    std::string tmp2 = extractNthWord(3, *it);
		    y2mil("tmp2:" << tmp2);
		    ppc_pegasos = boost::starts_with(tmp2, "Pegasos");
		}
	    }
	}

	if (is_ia64())
	{
	    efiboot = true;
	}
	else
	{
	    SystemCmd cmd(TESTBIN " -d '/sys/firmware/efi/vars'");
	    efiboot = cmd.retcode() == 0;
	}

	const char* tenv = getenv("LIBSTORAGE_EFI");
	if (tenv)
	{
	    efiboot = std::string(tenv) == "yes";
	}

	y2mil(*this);
    }


    bool
    Arch::is_ia64() const
    {
	return boost::starts_with(arch, "ia64");
    }


    bool
    Arch::is_ppc() const
    {
	return boost::starts_with(arch, "ppc");
    }


    bool
    Arch::is_ppc64le() const
    {
	return boost::starts_with(arch, "ppc64le");
    }


    bool
    Arch::is_s390() const
    {
	return boost::starts_with(arch, "s390");
    }


    bool
    Arch::is_sparc() const
    {
	return boost::starts_with(arch, "sparc");
    }


    bool
    Arch::is_x86() const
    {
	return arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" ||
	    arch == "x86_64";
    }


    std::ostream&
    operator<<(std::ostream& s, const Arch& arch)
    {
	return s << "arch:" << arch.arch << " ppc_mac:" << arch.ppc_mac
		 << " ppc_pegasos:" << arch.ppc_pegasos << " efiboot:"
		 << arch.efiboot;
    }

}
