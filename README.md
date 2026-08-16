# Maze-Solver-GROUP-05,CSE115
A program that generates random mazes and then solves them.
The Maze Solver is a programming project that generates random, solvable mazes and finds a path from the start to the destination using search algorithms such as DFS and BFS. The program visually displays the maze and the solving process, allowing users to observe how different algorithms explore the maze. It also compares algorithm performance based on factors like execution time, path length, and visited nodes, helping students understand graph traversal, algorithm design, and problem-solving techniques.

# 🧩 Random Maze Solver Game (CSE115)

A maze solver game that generates a **random maze in every run**. The project includes maze generation, pathfinding-based solving, and a clean modular code structure.

---

## 👥 Group Information

| No. | Name | ID |
| :---: | :--- | :---: |
| 01 | **Sadiba Ahmed** | `2623689042` |
| 02 | **Tawsif Bin Amin** | `2531243042` |
| 03 | **Gazi Haseen Abrar** | `2622956042` |
| 04 | **Nihad Binth Aktar** | `2421245643` |
| 05 | **Anika Siddiki** | `2422147610` |

*Course: **CSE115** | Section: **__** | Faculty: **__***  

---

## 🚀 Key Features
- **Random Maze Generation:** Generates a brand-new maze each time the program runs.
- **Guaranteed Solvable Maze:** Ensures a valid path exists from Start to End.
- **Maze Solver:** Solves the maze using a pathfinding algorithm (BFS / DFS / A*).
- **Solution Visualization:** Displays the solved path (and optionally step-by-step solving).
- **Replayability:** Regenerate mazes and try again without restarting (optional).

---

## 🎮 Controls (edit as needed)
- `W` — Move Up  
- `A` — Move Left  
- `S` — Move Down  
- `D` — Move Right  
- `P` — Show solution (optional)  
- `R` — Regenerate maze (optional)  
- `Q` — Quit  

---

## 🗃️ Repository Architecture
This repository follows a clean modular layout:

- `main.c` — Program entry point and main game loop  
- `maze.c` — Maze generation logic  
- `solver.c` — Maze solving logic (BFS/DFS/A*)  
- `render.c` — Maze drawing / printing logic  
- `input.c` — Keyboard input handling  
- `utils.c` — Helper functions (random seed, delay, file handling)

Optional folders:
- `data/` — Save files (best score, settings, etc.)
- `support/` — Extra modules (if needed)
- `others/` — Report, slides, demo video, documentation

---

## 🛠️ How to Compile and Run

### Prerequisites
- A C compiler installed (**GCC recommended**)


---

## ✅ How It Works (short explanation)
1. The program **generates a random maze** at the start of every run.
2. A **start (S)** and **end (E)** point are placed.
3. The solver algorithm finds a valid path from **S → E**.
4. The maze and/or the solution path is displayed.

---

## 📸 Screenshots / Demo
Add screenshots or a GIF here (optional):
- `others/demo.gif`
- `others/screenshot.png`

---

## 📌 Future Improvements \
- Add difficulty levels (maze size options)
- Add timer and scoring system
- Add step-by-step solving animation
- Add colored terminal output / graphics mode (SDL2)

---
