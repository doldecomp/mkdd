Contributing
============

Thanks for helping out! This guide explains how the decompilation works and the workflow this project follows. If anything is unclear, ask on [Discord](https://discord.gg/hKx3FJJgrV).

Make sure you can build the project first (see [README.md](README.md#building)).

How it works
============

The goal is to write C++ that, compiled with the original compiler (Metrowerks CodeWarrior), produces **byte-for-byte** the same code as the game.

1. **Split**: `ninja` extracts `sys/main.dol` from your disc image and splits it into one object file per translation unit (TU), using [`splits.txt`](config/MarioClub_us/splits.txt) and [`symbols.txt`](config/MarioClub_us/symbols.txt). These are the **target** objects.
2. **Compile**: Source files in `src/` are compiled with the original compiler and flags. These are the **base** objects.
3. **Diff**: [objdiff](https://github.com/encounter/objdiff) compares base and target, function by function, and shows a match percentage.
4. **Link**: Each TU is listed in [`configure.py`](configure.py) with a status:
   - `Matching`: your object is used to link the final DOL.
   - `NonMatching`: the original (extracted) object is used instead.
   - `Equivalent`: behaves the same but doesn't match exactly. Only linked with `--non-matching`.
5. **Verify**: `ninja` links the DOL and checks its SHA-1 against [`build.sha1`](config/MarioClub_us/build.sha1). If any `Matching` TU is wrong, the build fails.

A TU at 100% in objdiff is not done until it is flagged `Matching` **and** `ninja` succeeds.

Workflow
========

We work **one TU at a time**: one TU per pull request, taken to 100%.

1. **Pick a TU**
   - Prefer game code (`src/` folders like `Kaneshige`, `Sato`, `Inagaki`...) over SDK/JSystem.
   - Check the [open pull requests](https://github.com/doldecomp/mkdd/pulls) (including drafts) so you don't duplicate someone else's work.
2. **Open a draft PR** early so others know you're working on it.
3. **Decompile** until the TU reaches 100% in objdiff (code and data).
4. **Flag it** as `Matching` in `configure.py`:

   ```python
   Object(Matching, "Yamamoto/KartTumble.cpp"),
   ```

5. **Verify**:

   ```sh
   python configure.py
   ninja
   ```

   The build must succeed with no SHA-1 mismatch.
6. **Check for regressions** (optional but recommended): run `ninja baseline` on `main` before your changes, then `ninja changes` on your branch to list anything that got worse.
7. **Clean up** before review: sensible names for members, functions and locals, no leftover debug code, no unused variables.
8. **Mark the PR ready for review.** CI rebuilds the project and checks symbol order for the `.cpp` files you changed.

Using LLMs
==========

LLM tools (Copilot, Claude, OpenCode, local models...) are **allowed**, within these rules:

- **Same workflow.** One TU per PR, 100% matching, flagged `Matching`, `ninja` passing. Don't point an agent at the whole repository.
- **Disclose it.** Say in the PR description that an LLM was used, and for what.
- **Review it yourself.** You are responsible for the code you submit. Read it, understand it, and clean it up (names, types, structure) before asking for review. Maintainers shouldn't be the first humans to read it.
- **Matching is not enough.** Code that matches but uses wrong types, nonsense names or weird hacks will be asked to change.

In practice, LLMs often get most of a TU done but stall on the last few percent (register allocation, instruction ordering...). Expect to finish those by hand.
