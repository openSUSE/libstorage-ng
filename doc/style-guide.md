
Style Guide
===========


Class names should normally use the CapWords convention.

Function names should be lowercase with words separated by underscores.

  http://www.stroustrup.com/bs_faq2.html#Hungarian
  http://www.boost.org/development/requirements.html
  http://google-styleguide.googlecode.com/svn/trunk/cppguide.html
  https://www.python.org/dev/peps/pep-0008/
  https://github.com/styleguide/ruby


The auto keyword should be avoided.

  http://en.wikipedia.org/wiki/C%2B%2B11


C++11, C++14 and C++17 features can be used when the tool-chain for all
supported distributions provide them.


For API functions it must be avoided to modify parameters and use them as
return values since this is not always obvious for API users and difficult to
support in some target languages.

