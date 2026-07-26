← [Project milestones index](../milestones.md)

## Milestone 1 — End-to-end edit → compile → run

This document defines **Milestone 1**: the first product-useful Apollo IDE capability —
create and edit Pascal source, save and load files, compile with the Apollo Pascal
compiler, run the resulting Gemini bytecode on the Gemini VM, and surface diagnostics
and runtime output.

It complements:

- [Project milestones](../milestones.md)
- [Overview](../overview.md)
- [Milestone 0 — Project skeleton](00-project-skeleton.md)
- Sister projects **apollo-compiler** (`apolloc`) and **gemini-system** (standalone
  Gemini VM runner)

### Goals

- Prove the architectural model: Qt shell + external compiler process + external VM
  process.
- Deliver a complete single-file workflow: **edit → save → compile → run**.
- Display compiler diagnostics and program output in the IDE.
- Keep language and toolchain details behind adapters so later front-ends and path
  configuration can grow without rewriting the UI shell.

### Starting point

Milestone 0 provides a buildable Qt application and docs layout. Milestone 1 assumes
working installs (or local builds) of:

- Apollo Pascal Compiler (`apolloc` from apollo-compiler)
- Gemini VM executable (from gemini-system)

Toolchain paths may be hard-coded or set via simple settings for M1; polished
configuration UI can wait for a later milestone.

### Out of scope for Milestone 1

- Project / workspace multi-file management
- Syntax highlighting and language services
- Integrated debugger
- In-process compiler library integration
- Cross-platform packaging / installers
- Additional Apollo language front-ends beyond Pascal

---

## Milestone slices (summary)

| Slice | Focus |
|-------|--------|
| **M1a** | Editor: create, open, edit, save, load Pascal source |
| **M1b** | Compiler adapter: invoke `apolloc`, capture diagnostics |
| **M1c** | VM adapter: run bytecode, capture runtime output |
| **M1d** | Workflow polish, path settings, docs, M1 close-out |

Slices map 1:1 to Stages 1–4 below. Each stage should leave `main` buildable and update
the **Implementation status** section when closed.

---

## Staged delivery plan

### Stage 1 — Editor foundation (M1a) — completed

**Objective:** Use the IDE as a simple Pascal source editor with file persistence.

**Deliverables**

- `EditorWidget` (or equivalent) hosting a text editor for `.pas` (and plain text)
- Main-window actions: New, Open, Save, Save As
- Dirty-state tracking and basic unsaved-change prompts
- Status indication of the current file path (window title or status bar)

**Out of scope for Stage 1**

- Compile / run actions
- Syntax highlighting
- Multiple editor tabs (single document is enough)

**Acceptance criteria**

- [x] Create a new buffer, type Pascal source, save to disk
- [x] Open an existing `.pas` file and edit it
- [x] Save As writes a new path and subsequent Save uses that path
- [x] Closing with unsaved changes prompts the user

---

### Stage 2 — Compiler integration (M1b)

**Objective:** Compile the current file through the Apollo Pascal Compiler and show
results in the IDE.

**Deliverables**

- `CompilerAdapter` that launches `apolloc` (or configured compiler path) as an external
  process
- Compile action on the current saved file (prompt to save if dirty)
- Output pane (or equivalent) for compiler stdout/stderr
- Basic success/failure indication (exit status and/or message)
- Discovery of the produced bytecode artifact path for Stage 3 (convention or compiler
  output parsing — document the chosen rule)

**Out of scope for Stage 2**

- Running the VM
- Clickable diagnostic navigation (nice-to-have; not required)
- Multi-file / project builds

**Acceptance criteria**

- [ ] Compile a known-good example (e.g. apollo-compiler `hello.pas`) succeeds
- [ ] Compiler errors appear in the IDE output/diagnostics view
- [ ] Failed compile does not proceed to a run step
- [ ] Adapter fails cleanly if the compiler executable is missing or misconfigured

---

### Stage 3 — VM execution (M1c)

**Objective:** Run successfully compiled Gemini bytecode on the Gemini VM from the IDE.

**Deliverables**

- `VmAdapter` that launches the Gemini VM as an external process with the bytecode path
- Run action (compile-then-run, or run last successful artifact — document the choice)
- Capture and display VM stdout/stderr in an output pane
- Surface non-zero VM exit status to the user

**Out of scope for Stage 3**

- Interactive stdin beyond a minimal approach (defer rich console I/O if costly)
- Debugging, breakpoints, or step execution
- Parallel / background long-running process management UI beyond cancel-if-easy

**Acceptance criteria**

- [ ] After a successful compile, Run executes the program on the Gemini VM
- [ ] Program output appears in the IDE
- [ ] VM failures and missing VM executable are reported clearly
- [ ] User can distinguish compiler output from runtime output (separate panes or clear
      labelling)

---

### Stage 4 — Workflow polish & close-out (M1d)

**Objective:** Make the end-to-end path usable day-to-day and close the milestone.

**Deliverables**

- Simple toolchain path configuration (settings dialog, config file, or environment
  variables — pick one and document it)
- Menu/toolbar wiring for New / Open / Save / Compile / Run
- README or docs section: how to point the IDE at `apolloc` and the Gemini VM
- Update milestone index / implementation status; note known limitations

**Acceptance criteria**

- [ ] A new user can configure paths, open an example, compile, and run without reading
      source code
- [ ] End-to-end path works on at least one host platform used for Apollo development
- [ ] Milestone 1 marked completed in [`docs/milestones.md`](../milestones.md)

---

## Architecture notes (M1)

```
MainWindow
  ├── EditorWidget          // document buffer + editor UI
  ├── CompilerAdapter       // QProcess (or equivalent) → apolloc
  ├── VmAdapter             // QProcess (or equivalent) → gemini-vm
  └── Output / diagnostics  // panes fed by adapter signals
```

- Prefer **signals/slots** (or similar) so adapters stay UI-agnostic.
- Do not link against apollo-compiler or gemini-system libraries in M1.
- Single open document is the M1 document model; tabs/projects come later.

---

## Implementation status

| Slice | Status |
|-------|--------|
| M1a — Editor foundation | Completed |
| M1b — Compiler integration | Not started |
| M1c — VM execution | Not started |
| M1d — Workflow polish & close-out | Not started |

---

## Exit

Milestone 1 is complete when Stages 1–4 acceptance criteria are met: a developer can
create or open Pascal source in the IDE, compile it with Apollo, run it on the Gemini VM,
and see diagnostics and runtime output — without leaving the application.
