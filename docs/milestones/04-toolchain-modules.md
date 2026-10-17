← [Project milestones index](../milestones.md)

## Milestone 4 — Toolchain modules (maths & I/O)

This document defines **Milestone 4**: update the IDE’s compile and run adapters so they
match the current Apollo / Gemini CLI for programs that need maths and I/O support via
Gemini modules.

Target invocations:

```bash
# compile
apolloc --tbc program.pas -o program.tbc

# run (loads every module in that directory)
gemini-vm --modules /path/to/gemini/modules program.tbc
```

It complements:

- [Project milestones](../milestones.md)
- [Overview](../overview.md)
- [Milestone 1 — End-to-end edit → compile → run](01-end-to-end-mvp.md)
- Sister projects **apollo-compiler** (`apolloc`) and **gemini-system** (`gemini-vm`)

### Goals

- Compile with `apolloc --tbc … -o …` so the sibling `.tbc` is produced by the compiler.
- Run with `gemini-vm [--modules <dir>] …` so maths and I/O modules load when configured.
- Configure the modules directory via Preferences and/or environment, consistent with
  existing compiler/VM path resolution.
- Keep the M1–M3 edit → compile → run UX (active tab) unchanged aside from toolchain args.

### Starting point

Milestone 3 delivers multi-tab editing with Compile/Run on the active document. After
M4a–M4b, compile uses `apolloc --tbc … -o …` and run uses
`gemini-vm --modules <dir> <tbc>` when a modules path is configured.

### Out of scope for Milestone 4

- Project / workspace file trees
- Selecting individual modules (directory load is enough)
- Interactive program stdin / debugger
- Shipping Gemini modules inside the IDE package
- Changing editor, highlighter, or tab behaviour

---

## Milestone slices (summary)

| Slice | Focus |
|-------|--------|
| **M4a** | `CompilerAdapter`: `apolloc --tbc <source> -o <sibling.tbc>` |
| **M4b** | Modules path settings + `VmAdapter`: `gemini-vm --modules <dir> <tbc>` |
| **M4c** | Preferences UI, status/docs, M4 close-out (`0.4.0`) |

Slices map 1:1 to Stages 1–3 below. Each stage should leave `main` buildable and update
the **Implementation status** section when closed.

---

## Staged delivery plan

### Stage 1 — Compile CLI (M4a) — completed

**Objective:** Produce `.tbc` with the current `apolloc --tbc` interface.

**Deliverables**

- [`CompilerAdapter`](../../src/compileradapter.cpp) invokes  
  `apolloc --tbc <sourcePath> -o <sibling.tbc>` (working directory remains the source dir)
- On success, `lastBytecodePath()` is the `-o` path; stop writing bytecode from stdout
- Adapter tests updated for the new argument list and success path
- Document the CLI change relative to Milestone 1’s `--emit` note

**Implementation note:** Milestone 1’s `apolloc --emit` + IDE-written stdout `.tbc` path is
superseded; the compiler writes the artifact via `-o`.

**Acceptance criteria**

- [x] Compile of a saved `.pas` writes/updates a sibling `.tbc` via `-o`
- [x] Compiler diagnostics still appear on the Compiler output tab
- [x] Existing Compile / Run-from-IDE flow still chains on success

---

### Stage 2 — Modules path + VM CLI (M4b) — completed

**Objective:** Run bytecode with Gemini modules loaded.

**Deliverables**

- Modules directory resolution (same order as compiler/VM binaries):
  1. Environment variable `APOLLO_MODULES` if set
  2. Path saved in Preferences storage (`toolchain/modulesPath`)
  3. Unset — Run still launches the VM without `--modules` (maths/I/O may fail at runtime)
- [`VmAdapter`](../../src/vmadapter.cpp) invokes  
  `gemini-vm --modules <dir> <tbcPath>` when configured, otherwise `gemini-vm <tbcPath>`
- [`ToolchainSettings`](../../src/toolchainsettings.cpp) stores/resolves the modules path
- Main window applies the resolved path when configuring the VM adapter
- Tests for resolution order and VM argument list

**Acceptance criteria**

- [x] Run passes `--modules` with the resolved directory when set
- [x] Missing modules path omits `--modules` and still starts the VM
- [x] `APOLLO_MODULES` overrides a saved Preferences path

---

### Stage 3 — Preferences + close-out (M4c) — completed

**Objective:** Make modules configuration discoverable and close the milestone.

**Deliverables**

- Preferences field for Gemini modules directory (browse-able), beside compiler/VM paths
- Status bar or tooltip can surface the configured modules dir (lightweight is fine)
- README / milestone docs updated; Implementation status completed
- Version bump policy: report `0.4.0` when M4 closes (tag `v0.4.0` as a separate release
  step)

**Acceptance criteria**

- [x] M4a–M4b acceptance criteria met
- [x] Milestone 4 marked completed in [`docs/milestones.md`](../milestones.md)
- [x] README documents `--tbc` / `--modules` and `APOLLO_MODULES`

---

## Architecture notes (M4)

```
MainWindow
  ├── ToolchainSettings     // compiler, VM, modules paths (+ env overrides)
  ├── CompilerAdapter       // apolloc --tbc source -o sibling.tbc
  └── VmAdapter             // gemini-vm [--modules <dir>] program.tbc
```

- Keep adapters UI-agnostic; `MainWindow` / settings resolve paths and push them in.
- Shared Compiler / Runtime output tabs remain unchanged.
- Active-tab compile/run pinning from Milestone 3 stays as-is.

---

## Implementation status

| Slice | Status |
|-------|--------|
| M4a — Compile CLI (`--tbc` / `-o`) | Completed |
| M4b — Modules path + VM CLI | Completed |
| M4c — Preferences + close-out | Completed |

---

## Exit

Milestone 4 is complete when Stages 1–3 acceptance criteria are met: the IDE compiles with
`apolloc --tbc … -o …`, runs with `gemini-vm` (optionally `--modules …`), and users can
configure the modules directory via Preferences or `APOLLO_MODULES`.
