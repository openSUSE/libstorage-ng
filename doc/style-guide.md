
Style Guide
===========

* Class names should normally use the CapWords convention.

* Function names should be lowercase with words separated by underscores.

  http://www.stroustrup.com/bs_faq2.html#Hungarian
  http://www.boost.org/development/requirements.html
  http://google-styleguide.googlecode.com/svn/trunk/cppguide.html
  https://www.python.org/dev/peps/pep-0008/
  https://github.com/styleguide/ruby

* The auto keyword should be avoided.

  http://en.wikipedia.org/wiki/C%2B%2B11

  https://www.youtube.com/watch?v=HddFGPTAmtU&t=29m27s

* C++14 is used. Features of newer C++ standard can be used when the
  tool-chain for all supported distributions provides them.

* For API functions it must be avoided to modify parameters and use them as
  return values since this is not always obvious for API users and difficult
  to support in some target languages.

* Tabs are eight spaces wide. Use them for indentation.

* An indentation level is four spaces wide.


Conventions
===========

Functions used in the text() or commit() functions of Action XX should
be called do_xx_text() and do_xx(). E.g. the Mount action uses
Filesystem.do_mount() and Filesystem.do_mount_text(). No other
functions should have that naming schema (except helper function for
those functions).

For the do_xx_text() and do_xx() functions the preferable arguments
are the tense, the CommitData and the Action.


When variables or values with an underscores in the name are written to XML
the underscores have to be replaced by dashes. So e.g. chunk_size is saved as
&lt;chunk-size&gt;16384&lt;/chunk-size&gt;. In the logs the names should
follow the same convention.

