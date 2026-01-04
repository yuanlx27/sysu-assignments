I want you to adopt a strict "Long-Running Agent Harness" workflow to build this project. 

Please follow this exact protocol for all our interactions to manage context limits and ensure high-quality code generation.

### 1. THE STATE FILE (Memory)
First, check if a file named `agent_progress.md` exists.
- **If NO:** Create it. You will act as the "Initializer Agent". Analyze the current request/codebase and create a detailed, step-by-step implementation plan in this file. Use Markdown checkboxes (`- [ ]`).
- **If YES:** You are the "Coding Agent". Read `agent_progress.md` to restore your context. Do NOT rely solely on conversation history. This file is the source of truth.

### 2. THE WORKFLOW LOOP
For each interaction, you must follow this cycle:
1.  **LOAD:** Read `agent_progress.md` to identify the next unchecked task.
2.  **FOCUS:** Select *only* the next single atomic task. Do not try to do multiple steps at once.
3.  **EXECUTE:** Write the code for that specific task.
4.  **VERIFY:** 
    - Create or run a test script to verify this specific change works. 
    - Do not assume it works; prove it (e.g., `npm test`, `python manage.py test`, or a custom script).
5.  **COMMIT:** If verified, run `git add .` and `git commit -m "feat: [Task Name]..."`.
6.  **UPDATE:** specificly check the box in `agent_progress.md` to mark it as done and append a brief log of what was changed/decided.
7.  **PAUSE:** Stop and ask me for confirmation to proceed to the next task.

### 3. CONSTRAINTS
- **No Hallucination:** If you are unsure about a library or API, verify it by reading the docs or checking the environment first.
- **Atomic Progress:** Never mark a task as done in the progress file until the tests pass.
- **Self-Correction:** If a test fails, update the progress file with a "Blocked/Failed" note and attempt to fix it before moving on.

Now, please start by initializing or reading the `agent_progress.md` based on my request: 
- 按照图片中的要求，使用 Python 实现一个图像分割程序；
- 程序应该能读取给定路径的图片，计算出其前景与背景的“割”并画出“割”的轮廓，并将轮廓图输出为另一给定路径的图片，具体如图 `task-part-2` 所示；
- 所有代码写在一个 `assets/main.py` 文件夹中；
- 从零实现需要的网络流算法；
- 不要使用 Git。