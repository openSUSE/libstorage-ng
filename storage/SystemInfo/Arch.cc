/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2017-2020] SUSE LLC
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
#include "storage/Utils/XmlFile.h"


namespace storage
{

    // probing must be avoided for the Arch object in StorageImpl
    // TODO overall not nice

    Arch::Arch(bool do_probe)
	: arch("unknown"), ppc_mac(false), ppc_pegasos(false), ppc_power_nv(false),
	  efiboot(false), page_size(0)
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

	getChildValue(node, "page-size", page_size);
    }


    void
    Arch::saveData(xmlNode* node) const
    {
	setChildValue(node, "arch", arch);

	setChildValueIf(node, "efiboot", efiboot, efiboot);

	setChildValue(node, "page-size", page_size);
    }


    void
    Arch::probe()
    {
	SystemCmd cmd1(UNAME_BIN " -m", SystemCmd::DoThrow);

	if (cmd1.stdout().size() == 1)
	    arch = cmd1.stdout().front();

	if (is_ppc())
	{
	    AsciiFile cpuinfo("/proc/cpuinfo");
	    vector<string>::const_iterator it = find_if(cpuinfo.get_lines(), string_starts_with("machine\t"));
	    if (it != cpuinfo.get_lines().end())
	    {
		y2mil("line:" << *it);

		string tmp1 = extractNthWord(2, *it);
		y2mil("tmp1:" << tmp1);
		ppc_mac = boost::starts_with(tmp1, "PowerMac") || boost::starts_with(tmp1, "PowerBook");
		ppc_pegasos = boost::starts_with(tmp1, "EFIKA5K2");
                ppc_power_nv = boost::starts_with(tmp1, "PowerNV");

		if (!ppc_mac && !ppc_pegasos)
		{
		    string tmp2 = extractNthWord(3, *it);
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
	    SystemCmd::Options options(TEST_BIN " -d '/sys/firmware/efi/vars'");
	    options.throw_behaviour = SystemCmd::DoThrow;
	    options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 1; };

	    SystemCmd cmd(options);

	    efiboot = cmd.retcode() == 0;
	}

	const char* tenv = getenv("LIBSTORAGE_EFI");
	if (tenv)
	{
	    efiboot = string(tenv) == "yes";
	}

	SystemCmd cmd2(GETCONF_BIN " PAGESIZE", SystemCmd::DoThrow);

	if (cmd2.stdout().size() == 1)
	    cmd2.stdout().front() >> page_size;

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
	return s << "arch:" << arch.arch << " ppc-mac:" << arch.ppc_mac << " ppc-pegasos:"
		 << arch.ppc_pegasos << " ppc-power-nv:" << arch.ppc_power_nv << " efiboot:"
		 << arch.efiboot << " page-size:" << arch.page_size;
    }

}
