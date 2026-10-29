//
// Ruby SWIG interface definition for libstorage-ng
//

%{
#include <sstream>

#include "storage/Utils/Logger.h"

namespace storage
{

    /**
     * Logger that is set as the current logger instead of the logger set from Ruby (usually a
     * director object).
     *
     * Since SWIG 4.5 the destructor of a wrapped object, e.g. of a Storage, can be run by the
     * garbage collector of Ruby while sweeping. Destructors can log and, since the logger is
     * implemented in Ruby, that executes Ruby code, which is not allowed during the garbage
     * collection (Ruby aborts with "object allocation during garbage collection phase").
     *
     * So during the garbage collection nothing is logged and the logger from Ruby is not called.
     */
    class GcSafeLogger : public Logger
    {
    public:

	Logger* get_target() const { return target; }
	void set_target(Logger* target) { this->target = target; }

	virtual bool test(LogLevel log_level, const std::string& component) override
	{
	    if (!target || rb_during_gc())
		return false;

	    return target->test(log_level, component);
	}

	virtual void write(LogLevel log_level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) override
	{
	    if (!target || rb_during_gc())
		return;

	    target->write(log_level, component, file, line, function, content);
	}

    private:

	Logger* target = nullptr;

    };


    static GcSafeLogger gc_safe_logger;


    Logger* get_ruby_logger()
    {
	return gc_safe_logger.get_target();
    }


    void set_ruby_logger(Logger* logger)
    {
	gc_safe_logger.set_target(logger);

	set_logger(logger ? &gc_safe_logger : nullptr);
    }

}
%}

// Storage.logger and Storage.logger= use the GcSafeLogger. The wrapper code (argument
// conversion, return value) is not changed, only the called function.

%feature("action") storage::get_logger
{
    result = (storage::Logger*) storage::get_ruby_logger();
}

%feature("action") storage::set_logger
{
    storage::set_ruby_logger(arg1);
}

%rename("empty?") "empty";

%rename("%(regex:/^(get_)(.*)/\\2/)s") "";
%rename("%(regex:/^(set_)(.*)/\\2=/)s") "";
%rename("%(regex:/^(is_)(.*)/\\2?/)s") "";

%rename("device_exists?") "device_exists";

%rename("exists_in_devicegraph?") "exists_in_devicegraph";
%rename("exists_in_probed?") "exists_in_probed";
%rename("exists_in_staging?") "exists_in_staging";
%rename("exists_in_system?") "exists_in_system";

%rename("exists_pool?") "exists_pool";

// TODO rename all supports_*, maybe more

%rename("supports_mount?") "supports_mount";

%define use_ostream(CLASS)

%extend CLASS
{
    std::string to_s()
    {
	std::ostringstream out;
	out << *($self);
	return out.str();
    }
};

%enddef

%include "../storage.i"

