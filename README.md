# CPLib Initializers

This repository includes initializers officially maintained by CPLib.

## How to use

First include [cplib.hpp](https://github.com/rindag-devs/cplib) in the code, then select an initializer for your target platform, and use the corresponding `CPLIB_REGISTER_<MODE>_OPT` Macro registration state with the initializer.

Here's a usage example:

```cpp
#include "cplib.hpp"

#include "cplib-initializers/include/testlib/checker.cpp"

// Some initializers have non-default constructors where parameters can be passed to the constructor
CPLIB_REGISTER_CHECKER_OPT(Input, Output, cplib_initializers::testlib::checker::Initializer(true));
```

All initializers in this project are independent. You can include a single file into your program without including the entire project.

## Platform Compatibility

See [platform_compatibility.md](platform_compatibility.md) for details.

## License

[LGPL-3.0-or-later][license]

Copyright (c) 2024-present, rindag-devs

[license]: https://github.com/rindag-devs/cplib-initializers/blob/main/COPYING.LESSER
