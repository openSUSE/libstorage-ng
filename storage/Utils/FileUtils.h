#ifndef STORAGE_FILE_UTILS_H
#define STORAGE_FILE_UTILS_H


#include <string>


namespace storage
{

    using std::string;


    class TmpDir
    {

    public:

	/**
	 * Creates a temporary directory. For details see mkdtemp.
	 */
	TmpDir(const string& name_template);

	/**
	 * Deletes the temporary directory. Does not try to delete files
	 * inside the temporary directory.
	 */
	~TmpDir();

	const string& get_path() const { return path; }
	const string& get_name() const { return name; }

	string get_fullname() const { return path + "/" + name; }

    protected:

	string path;
	string name;

    };

}


#endif
