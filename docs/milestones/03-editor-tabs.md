← [Project milestones index](../milestones.md)

## Milestone 3 — Editor tabs

This document defines **Milestone 3**: move from a single open document to multiple
editor tabs so developers can work across several Pascal (and plain-text) files in one
window, while keeping the M1 compile/run model (active document only).

It complements:

- [Project milestones](../milestones.md)
- [Overview](../overview.md)
- [Milestone 1 — End-to-end edit → compile → run](01-end-to-end-mvp.md)
- [Milestone 2 — Editor polish](02-editor-polish.md)

### Goals

- Open and edit multiple files via tabs without a project/workspace model yet.
- Preserve dirty tracking, save prompts, and title/status behaviour per document.
- Compile and Run target the **current tab** only (same adapters as M1).
- Reuse Milestone 2 editor features (highlighter, line numbers) in every tab.

### Starting point

Milestone 1 provides the single-document shell and toolchain adapters. Milestone 2 adds
editor polish intended to live on `EditorWidget` so each tab can host one instance.

### Out of scope for Milestone 3

- Project / workspace folders and file trees
- Multi-file build pipelines / unit graphs
- Split editors or drag-out windows
- Debugger
- Language servers

---

## Milestone slices (summary)

| Slice | Focus |
|-------|--------|
| **M3a** | Tab widget hosting multiple `EditorWidget`s; current-document API |
| **M3b** | New / Open / Close tab; dirty close prompts; window title from current tab |
| **M3c** | Compile / Run / Save wired to the active tab only |
| **M3d** | Polish (tab titles, reopen behaviour notes), docs, M3 close-out |

Slices map 1:1 to Stages 1–4 below. Each stage should leave `main` buildable and update
the **Implementation status** section when closed.

---

## Staged delivery plan

### Stage 1 — Tab shell (M3a)

**Objective:** Replace the single central editor with a `QTabWidget` of editors.

**Deliverables**

- Document model: one `EditorWidget` (or thin wrapper) per tab
- `currentEditor()` / current path / dirty helpers on `MainWindow` (or a small controller)
- Opening the app still presents one empty/untitled tab

**Acceptance criteria**

- [ ] At least one editor tab is always present at startup
- [ ] Switching tabs changes which widget has focus
- [ ] Milestone 2 highlighting/line numbers still work in a tab

---

### Stage 2 — Open / close / dirty (M3b)

**Objective:** Manage tab lifetime safely.

**Deliverables**

- **New** creates a new untitled tab (or clears policy — prefer new tab)
- **Open** adds a tab for the chosen file (if already open, switch to that tab)
- **Close** tab action (and tab close button if enabled) with dirty Save / Discard / Cancel
- Window title reflects the **current** tab’s name and dirty marker
- Closing the window prompts for **each** dirty tab (or an aggregated prompt — document
  the chosen behaviour; per-tab prompts are acceptable for M3)

**Acceptance criteria**

- [ ] Open two files in two tabs and edit both independently
- [ ] Closing a dirty tab prompts; Cancel leaves the tab open
- [ ] Re-opening an already-open path focuses the existing tab

---

### Stage 3 — Active-tab build/run (M3c)

**Objective:** Keep compile/run semantics clear with multiple documents.

**Deliverables**

- Save / Save As / Compile / Run operate on the current tab only
- Compile still writes sibling `.tbc` beside that tab’s path
- Run remains compile-then-run for the current tab
- Disable or no-op build actions gracefully when no current editor exists

**Acceptance criteria**

- [ ] Compiling tab A does not save or overwrite tab B’s buffer
- [ ] Run on the current tab executes that file’s bytecode after a successful compile
- [ ] Switching tabs after compile does not auto-run another tab

---

### Stage 4 — Polish & close-out (M3d)

**Objective:** Finish multi-tab UX and close the milestone.

**Deliverables**

- Tab title shows basename (with `*` when dirty)
- Optional: middle-click or shortcut to close tab
- Docs updated; Implementation status completed
- Version bump policy: report `0.3.0` when M3 closes (tag `v0.3.0` as a separate release
  step)

**Acceptance criteria**

- [ ] M3a–M3c acceptance criteria met
- [ ] Milestone 3 marked completed in [`docs/milestones.md`](../milestones.md)

---

## Architecture notes (M3)

```
MainWindow
  ├── QTabWidget
  │     ├── EditorWidget (doc A)
  │     └── EditorWidget (doc B)
  ├── CompilerAdapter          // unchanged; fed current path
  ├── VmAdapter                // unchanged; fed current .tbc
  └── Output tabs              // Compiler / Runtime (shared)
```

- Prefer **one shared** compiler/runtime output pane for all tabs (not per-tab consoles).
- Keep adapters UI-agnostic; `MainWindow` selects the active document path.
- Leave room for a later project model to open files into the same tab widget.

---

## Implementation status

| Slice | Status |
|-------|--------|
| M3a — Tab shell | Not started |
| M3b — Open / close / dirty | Not started |
| M3c — Active-tab build/run | Not started |
| M3d — Polish & close-out | Not started |

---

## Exit

Milestone 3 is complete when Stages 1–4 acceptance criteria are met: users can open
several Pascal files in tabs, edit them with dirty tracking, and compile/run the active
tab using the existing Apollo toolchain integration.
