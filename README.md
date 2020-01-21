# CMPT 373 Project

Spring 2020, Group Arepa



## Building

A wrapper around `cmake` and `make` is provided for convenience.

```bash
./build all
ls target/[platform string]/[build type]
```

| Option                                       | Description                                |
| -------------------------------------------- | ------------------------------------------ |
| `--debug`                                    | Creates a debug build.                     |
| `--release`                                  | Creates a release build.                   |
| `--reconfigure`                              | Reconfigures the project.                  |
| `--toolchain=/path/to/cmake/toolchain.cmake` | Specifies a toolchain for cross-compiling. |
| `--print-info`                               | Prints project and build information.      |

The `--debug`, `--release`, and `--toolchain` options will be automatically applied to subsequent invocations of the wrapper.



## License / Contributing

See [the contributing guide](./CONTRIBUTING.md) and [license](./LICENSE.md).



## Maintainers

| Contributor       | Role     |
| ----------------- | -------- |
| Anthony Tran      | \<Role\> |
| Ethan Pini        | \<Role\> |
| Harry Ceong       | \<Role\> |
| Justin Liu        | \<Role\> |
| Mehtab Singh Gill | \<Role\> |
| Nikola Maeda      | \<Role\> |
| Ryan Lintott      | \<Role\> |
| Sami El Feki      | \<Role\> |

