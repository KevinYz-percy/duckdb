# DuckDB Custom gRPC Wrapper

This project implements a simple **gRPC-based server** that wraps a DuckDB instance, allowing remote execution of SQL queries.

It is designed to link against a **local source build** of DuckDB, enabling you to modify DuckDB's core source code and immediately see those changes reflected in the gRPC server.

## Prerequisites

*   **CMake** (3.15+)
*   **gRPC** & **Protobuf** (`brew install grpc protobuf`)
*   **DuckDB dependencies** (`brew install cmake`) - *Note: exact list depends on DuckDB build requirements if compiling from source.*

## Directory Structure

```
grpc_server/
├── CMakeLists.txt       # Build configuration
├── protos/
│   └── database.proto   # gRPC Service Definition
└── src/
    ├── server.cpp       # Server implementation (wraps DuckDB)
    └── client.cpp       # Client CLI tool
```

## 1. Build DuckDB from Source

Before building the wrapper, you must build DuckDB itself so we can link against it.

1.  Navigate to the DuckDB root directory:
    ```bash
    cd ../  # Assuming you are in grpc_server/
    # OR
    cd /path/to/duckdb_repo
    ```
2.  Run `make` to compile the release version:
    ```bash
    make
    ```
    *This creates libraries in `build/release/src` and header files in `src/include`.*

## 2. Build the gRPC Wrapper

1.  Navigate to the `grpc_server` directory:
    ```bash
    cd grpc_server
    ```
2.  Create a build directory:
    ```bash
    mkdir -p build && cd build
    ```
3.  Run CMake:
    ```bash
    cmake ..
    ```
    *This looks for the DuckDB build in `../../build/release`. if your DuckDB repo is elsewhere, edit `CMakeLists.txt` `DUCKDB_ROOT`.*
4.  Compile:
    ```bash
    make
    ```

## 3. Usage

### Start the Server
```bash
./duckdb_server
```
*It listens on `0.0.0.0:50051`.*

### Run the Client
In a separate terminal:
```bash
./duckdb_client "SELECT 'Hello, World!'"
```

## 4. Modifying DuckDB Source

To verify or use your own DuckDB modifications:

1.  **Edit Source**: Open any file in the DuckDB repo (e.g., `src/main/connection.cpp`).
2.  **Recompile DuckDB**: Run `make` in the DuckDB root.
3.  **Restart Server**: Just restart `./duckdb_server`. No need to recompile the server itself, as it links dynamically to the updated `libduckdb.dylib`.

## Troubleshooting

*   **DuckDB not found**: Ensure you ran `make` in the DuckDB root and that `build/release/src/libduckdb.dylib` exists.
*   **Protobuf/gRPC errors**: Ensure they are installed via Homebrew and linked correctly.
