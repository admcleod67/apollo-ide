← [Project milestones index](../milestones.md)

## Milestone 0 — Project skeleton

This document defines **Milestone 0**: the Apollo IDE repository and application
bootstrap. It establishes build, layout, and documentation so Milestone 1 can add real
edit → compile → run behaviour without reworking foundations.

It complements:

- [Project milestones](../milestones.md)
- [Overview](../overview.md)
- [Milestone 1 — End-to-end edit → compile → run](01-end-to-end-mvp.md)

### Goals

- Stand up a buildable Qt 6 + CMake application that opens a main window.
- Establish the initial source, resource, CMake, and docs layout described in the README.
- Provide overview and milestone documentation in the apollo-compiler style.
- Leave clear extension points for editor, compiler adapter, and VM adapter work in M1.

### Out of scope

- Source editing, file open/save, or document management
- Invoking the Apollo compiler or Gemini VM
- Syntax highlighting, projects, or debugging
- Packaging / installers

---

## Deliverables

### Repository and build

- Top-level `CMakeLists.txt` for a Qt 6 Widgets (or Quick, if chosen) application target
- CMake helpers under `cmake/` as needed (e.g. Qt find helpers, common options)
- `.gitignore` suitable for CMake/Qt build trees
- Application builds and launches on at least one supported host (macOS / Linux / Windows)

### Application shell

- `src/main.cpp` — application entry
- `src/mainwindow.cpp` / `mainwindow.h` — empty or placeholder main window
- Stub headers/sources (or documented placeholders) for:
  - `editorwidget`
  - `compileradapter`
  - `vmadapter`
- Minimal `resources/` layout (`icons/`, `styles/`) even if mostly empty

### Documentation

- `docs/README.md`, `docs/overview.md`, `docs/milestones.md`
- `docs/milestones/00-project-skeleton.md` (this file)
- `docs/milestones/01-end-to-end-mvp.md`
- Root `README.md` remains the public project summary; docs hold the working roadmap

---

## Acceptance criteria

- [ ] `cmake` configure + build succeeds with Qt 6 available
- [ ] Running the IDE binary shows a main window (title/menu stub is enough)
- [ ] Repository layout matches the initial structure in the README (or an explicitly
      documented evolution of it)
- [ ] Milestone 0 and Milestone 1 detail pages exist and are linked from the milestones index
- [ ] README or docs note how to configure and build the skeleton

---

## Exit

When the acceptance criteria are met, mark this milestone **completed** in
[`docs/milestones.md`](../milestones.md) and begin Milestone 1 slices.
