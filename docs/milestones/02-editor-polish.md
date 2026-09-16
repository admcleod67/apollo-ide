← [Project milestones index](../milestones.md)

## Milestone 2 — Editor polish

This document defines **Milestone 2**: raise the day-to-day editing experience so Apollo
IDE feels like a focused Pascal IDE. It covers toolbar icon conventions, Pascal syntax
highlighting, and a small set of editor affordances that do not change the single-document
model (tabs are Milestone 3).

It complements:

- [Project milestones](../milestones.md)
- [Overview](../overview.md)
- [Milestone 1 — End-to-end edit → compile → run](01-end-to-end-mvp.md)
- [Milestone 3 — Editor tabs](03-editor-tabs.md)

### Goals

- Align Run/Compile toolbar cues with common IDE conventions (Play = Run).
- Add Pascal syntax highlighting so source is easier to read and navigate.
- Add light editor chrome (line numbers; optional find) without projects or multi-doc UI.
- Keep the M1 compile/run adapters unchanged except where diagnostics UX benefits.

### Starting point

Milestone 1 delivers single-file edit → compile → run, Preferences for toolchain paths,
and a toolbar using Qt standard icons (including media icons for Compile/Run).

### Out of scope for Milestone 2

- Multiple open documents / tabs (Milestone 3)
- Project / workspace file trees
- Debugger or interactive program stdin
- Full language services (completion, rename, semantic highlighting)
- Custom icon artwork beyond fixing standard-icon usage (optional custom hammer later)

---

## Milestone slices (summary)

| Slice | Focus |
|-------|--------|
| **M2a** | Toolbar icon conventions (Play for Run; Compile without media Play) |
| **M2b** | Pascal `QSyntaxHighlighter` (keywords, comments, strings, numbers) |
| **M2c** | Line numbers in the editor gutter |
| **M2d** | Polish: find-in-document (optional if cheap), docs, M2 close-out |

Slices map 1:1 to Stages 1–4 below. Each stage should leave `main` buildable and update
the **Implementation status** section when closed.

---

## Staged delivery plan

### Stage 1 — Toolbar icon conventions (M2a) — completed

**Objective:** Make toolbar icons match common IDE expectations.

**Deliverables**

- **Run** uses a Play-style icon (`QStyle::SP_MediaPlay` or equivalent).
- **Compile** does not use Play; temporarily uses `QStyle::SP_MediaSeekForward` until a
  custom build/hammer asset is added under `resources/icons/`.
- New / Open / Save icons unchanged unless they conflict.

**Acceptance criteria**

- [x] Run toolbar button visually reads as “run/play”
- [x] Compile is visually distinct from Run
- [x] Shortcuts and menu labels unchanged

---

### Stage 2 — Pascal syntax highlighting (M2b)

**Objective:** Colour Pascal source in the editor.

**Deliverables**

- A `PascalHighlighter` (`QSyntaxHighlighter`) attached to the editor document
- Highlight at least: keywords, line/block comments, string literals, numbers
- Keyword set sufficient for Apollo Pascal as used today (extendable list)
- Sensible default colours that work on the default light palette (theme system later)

**Out of scope for Stage 2**

- Semantic / AST-based highlighting
- User-configurable colour schemes

**Acceptance criteria**

- [ ] Opening a `.pas` example shows highlighted keywords and comments
- [ ] Editing and undo keep highlighting consistent
- [ ] Non-Pascal / plain buffers remain usable (highlighter on by default for `.pas`, or
      always-on with Pascal rules — document the choice)

---

### Stage 3 — Line numbers (M2c)

**Objective:** Show 1-based line numbers beside the editor.

**Deliverables**

- Gutter or side widget synced to scroll and font metrics
- Updates on document change and resize

**Acceptance criteria**

- [ ] Line numbers match editor lines for a multi-line file
- [ ] Scrolling keeps gutter aligned

---

### Stage 4 — Find + close-out (M2d)

**Objective:** Finish editor polish and close the milestone.

**Deliverables**

- Basic find-in-document (Ctrl+F / Cmd+F) if it fits cleanly; otherwise defer explicitly
- README / milestone docs updated; Implementation status completed
- Version bump policy: report `0.2.0` when M2 closes (tag `v0.2.0` as a separate release
  step)

**Acceptance criteria**

- [ ] M2a–M2c acceptance criteria met
- [ ] Milestone 2 marked completed in [`docs/milestones.md`](../milestones.md)

---

## Architecture notes (M2)

```
EditorWidget
  ├── QPlainTextEdit
  ├── PascalHighlighter      // QSyntaxHighlighter on the document
  └── LineNumberArea         // optional companion widget
```

- Prefer keeping highlighting and gutter inside/near `EditorWidget` so Milestone 3 tabs
  can reuse one editor type per document.
- Do not couple highlighting to `apolloc` (no compiler round-trip for colours in M2).

---

## Implementation status

| Slice | Status |
|-------|--------|
| M2a — Toolbar icon conventions | Completed |
| M2b — Pascal syntax highlighting | Not started |
| M2c — Line numbers | Not started |
| M2d — Find + close-out | Not started |

---

## Exit

Milestone 2 is complete when Stages 1–4 acceptance criteria are met: Run looks like Run,
Pascal source is highlighted, line numbers are visible, and docs/version policy for `0.2.0`
are recorded.
