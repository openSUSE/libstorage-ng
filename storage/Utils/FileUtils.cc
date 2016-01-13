

#include <string.h>
#include <unistd.h>

#include "storage/Utils/FileUtils.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/ExceptionImpl.h"


namespace storage
{

    TmpDir::TmpDir(const string& name_template)
	: path(), name(name_template)
    {
	const char* tmpdir = getenv("TMPDIR");
	path = tmpdir ? tmpdir : "/tmp";

	char* t = strdup(get_fullname().c_str());
	if (!t)
	{
	    ST_THROW(Exception("strdup failed"));
	}

	if (mkdtemp(t) == NULL)
	{
	    free(t);

	    ST_THROW(Exception(sformat("mkdtemp failed, %s", strerror(errno))));
	}

	name = string(&t[strlen(t) - name.size()]);

	free(t);
    }


    TmpDir::~TmpDir()
    {
	if (rmdir(get_fullname().c_str()) != 0)
	{
	    y2err("rmdir '" << get_fullname() << "' failed, " << strerror(errno));
	}
    }

}
