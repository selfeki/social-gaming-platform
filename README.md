# CMPT 373 Project

Spring 2020, Group Arepa



## Building

A wrapper around `cmake` and `make` is provided for convenience.

```bash
./build --no-lint
ls target/[platform string]/[build type]
```

| Option                                       | Description                                  |
| -------------------------------------------- | -------------------------------------------- |
| `--debug`                                    | Creates a debug build.                       |
| `--release`                                  | Creates a release build.                     |
| `--reconfigure`                              | Reconfigures the project.                    |
| `--cores=4`                                  | Specifies the number of cores to build with. |
| `--toolchain=/path/to/cmake/toolchain.cmake` | Specifies a toolchain for cross-compiling.   |
| `--print-info`                               | Prints project and build information.        |

The `--debug`, `--release`, and `--toolchain` options will be automatically applied to subsequent invocations of the wrapper.

### Dependencies

- Cmake `3.14` or newer
- Clang `10` or newer
- Boost `1.72.0`
- `nlohmann_json`


## Running

To the run the server:

```bash
./build --no-lint && ./build --run=chatserver
```


## License / Contributing

See [the contributing guide](./CONTRIBUTING.md) and [license](./LICENSE.md).



## Maintainers

| Contributor       | Role            |
| ----------------- | --------------- |
| Sami El Feki      | Project Manager |
| Ethan Pini        | Technical Lead  |
| Anthony Tran      | Developer       |
| Harry Ceong       | Developer       |
| Justin Liu        | Developer       |
| Mehtab Singh Gill | Developer       |
| Nikola Maeda      | Developer       |
| Ryan Lintott      | Developer       |
