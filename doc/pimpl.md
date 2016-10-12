
Pimpl
-----

The pimpl implementation of libstorage-ng is simple. The object in the public
interface keeps a pointer to a unique implementation object. There is no
copy-on-write involved.

As [Herb Sutter explains](https://www.youtube.com/watch?v=JfmTagWcqoE) the
implementation uses ```const unique_ptr<T>```.

