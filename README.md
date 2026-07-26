# Apollo IDE

The Apollo IDE is a cross-platform Qt-based integrated development environment designed for building software that targets the Gemini VM. Its initial focus is supporting the Apollo Pascal Compiler, with planned expansion to additional Apollo language front-ends as they are developed.

## Overview

Apollo IDE provides a modern editing and build experience for Apollo-targeted languages while remaining fully independent of the Gemini System architecture. The IDE communicates with the apollo-compiler toolchain and executes compiled programs using the Gemini VM, which is distributed separately as part of the gemini-system project.

The IDE is designed to evolve iteratively and incrementally alongside the compiler and VM, allowing new features, language services, and debugging capabilities to be added organically.

## Current Status

The project is in its early stages. The initial MVP will provide:

- Creation and editing of Pascal source files
- Saving and loading files
- Invoking the Apollo Pascal Compiler
- Running compiled Gemini bytecode on the Gemini VM
- Displaying compiler diagnostics and runtime output

Future versions will expand on this foundation with project management, syntax highlighting, debugging support, multi-file builds, and additional language front-ends.

## Goals

- Deliver a clean, simple, cross-platform IDE for Apollo compilers
- Provide a unified workflow: edit → compile → run
- Maintain architectural independence from the Gemini System
- Support multiple Apollo language compilers as they are developed
- Integrate with the Gemini VM through a stable external process interface
- Grow organically through iterative feature development

## Architecture

Apollo IDE is structured as a standalone Qt application that interacts with external tools:

- **Apollo Compiler**
  - Invoked as a command-line process
  - Produces Gemini bytecode
  - Future integration via a shared compiler services library
  - See: compiler services
- **Gemini VM**
  - Executed as an external runtime
  - Runs Gemini bytecode produced by Apollo compilers
  - Platform-portable and independent of the IDE
  - See: Gemini VM integration

This separation ensures that the IDE remains flexible, portable, and language-agnostic.

## Planned Features

- Pascal syntax highlighting
- Project/workspace support
- Build pipelines for multi-file programs
- Integrated Gemini VM debugger
- Language service modules for future Apollo compilers
- Configurable toolchain paths
- Cross-platform packaging

## Repository Structure (initial)

```
apollo-ide/
  src/
    main.cpp
    mainwindow.cpp/.h
    editorwidget.cpp/.h
    compileradapter.cpp/.h
    vmadapter.cpp/.h
  resources/
    icons/
    styles/
  cmake/
  docs/
```

This structure will expand as new features are introduced.

## Requirements

- Qt 6 (or later)
- CMake
- Apollo Pascal Compiler (from apollo-compiler)
- Gemini VM executable (from gemini-system)

## License

This project is licensed under the [MIT License](LICENSE).

## Contributing

This project is not currently accepting external contributions.
