#ifndef STORAGE_STORAGE_H
#define STORAGE_STORAGE_H


#include <string>
#include <vector>
#include <list>
#include <memory>
#include <boost/noncopyable.hpp>


namespace storage
{
    /**
     * \mainpage libstorage Documentation
     *
     * \section Thread-Safety Thread Safety
     *
     * There is no guarantee about the thread safety of libstorage.
     *
     * \section Exceptions-and-Side-Effects Exceptions and Side Effects
     *
     * There is no guarantee that functions have no side effects if an
     * exception is thrown unless stated differently.
     *
     * \section Locking Locking
     *
     * During initialisation libstorage installs a global lock so that several
     * programs trying to use libstorage at the same time do not
     * interfere. This lock is either read-only or read-write depending on the
     * read_only parameter used in \link storage::Environment() \endlink.
     *
     * Several processes may hold a read-lock, but only one process may hold a
     * read-write lock. An read-write lock excludes all other locks, both
     * read-only and read-write.
     *
     * The support for multiple read-only locks is experimental.
     *
     * Locking may also fail for other reasons, e.g. limited permissions.
     */


    class Environment;
    class Arch;
    class Devicegraph;
    class Actiongraph;


    namespace Action
    {
	class Base;
    }


    class CommitCallbacks
    {
    public:

	virtual ~CommitCallbacks() {}

	virtual void message(const std::string& message) const = 0;
	virtual bool error(const std::string& message, const std::string& what) const = 0;

	// TODO to make pre and post generally usable the Action classes must
	// be included in the public libstorage interface
	virtual void pre(const Action::Base* action) const {}
	virtual void post(const Action::Base* action) const {}

    };


    //! The main entry point to libstorage.
    class Storage : private boost::noncopyable
    {
    public:

	Storage(const Environment& environment);
	~Storage();

    public:

	const Environment& get_environment() const;
	const Arch& get_arch() const;

	Devicegraph* create_devicegraph(const std::string& name);
	Devicegraph* copy_devicegraph(const std::string& source_name, const std::string& dest_name);
	void remove_devicegraph(const std::string& name);
	void restore_devicegraph(const std::string& name);

	bool equal_devicegraph(const std::string& lhs, const std::string& rhs) const;

	bool exist_devicegraph(const std::string& name) const;
	std::vector<std::string> get_devicegraph_names() const;

	Devicegraph* get_devicegraph(const std::string& name);
	const Devicegraph* get_devicegraph(const std::string& name) const;

	Devicegraph* get_staging();
	const Devicegraph* get_staging() const;

	const Devicegraph* get_probed() const;

	void check() const;

	const std::string& get_rootprefix() const;
	void set_rootprefix(const std::string& rootprefix);

	/**
	 * The actiongraph is only valid until either the probed or staging
	 * devicegraph is modified.
	 */
	const Actiongraph* calculate_actiongraph();

	/**
	 * The actiongraph must be valid.
	 */
	void commit(const CommitCallbacks* commit_callbacks = nullptr);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif
