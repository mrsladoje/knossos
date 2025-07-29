# Robot in Knossos 🤖🏛️

A C++ console-based interactive maze game inspired by the legendary Labyrinth of Knossos from Greek mythology. Guide Theseus's robot through a dynamically generated maze while avoiding the Minotaur and using special items to reach the exit.

## 🎮 Game Overview

The Robot in Knossos combines strategy, randomness, and interactive gameplay through a console interface. Players must navigate a robot through a procedurally generated maze, avoiding the dangerous Minotaur while collecting and using special items that provide temporary advantages.

### Key Features

- **Dynamic Maze Generation**: Uses Randomized Prim's algorithm to create unique mazes each time
- **Interactive Robot Control**: WASD movement controls with real-time response
- **Intelligent Minotaur AI**: The Minotaur moves randomly but will hunt the robot when nearby
- **Special Items System**: Four unique items with 3-turn duration effects
- **Fog of War**: Visibility-limiting mechanic that adds strategic depth
- **Performance Monitoring**: Built-in timing for maze generation analysis
- **Game State Persistence**: Automatic saving of game results with timestamps

## 🛠️ Technical Implementation

### Architecture

The project follows object-oriented design principles with clean separation of concerns:

- **Matrix Class**: Handles maze generation and field management
- **Gameplay Class**: Manages game flow and user interaction
- **MatrixField Hierarchy**: Polymorphic system for different field types
- **FileHandler**: Manages game state persistence
- **ConsoleHandler**: Optimized ANSI-based display updates

### Algorithms Used

- **Randomized Prim's Algorithm**: For maze generation ensuring connectivity

## 🎯 Special Items

| Item | Symbol | Effect | Duration |
|------|---------|---------|----------|
| **Fog of War** | 🌫️ | Limits visibility to 3x3 area around robot | 3 turns |
| **Sword** | ⚔️ | Allows robot to defeat the Minotaur | 3 turns |
| **Shield** | 🛡️ | Repels Minotaur to 2 squares distance | 3 turns |
| **Hammer** | 🔨 | Enables passage through interior walls | 3 turns |

## 🚀 Getting Started

### Prerequisites

- C++ compiler with C++11 support or later
- Terminal/Console with ANSI escape sequence support

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/robot-in-knossos.git
cd robot-in-knossos
```

2. Compile the project:
```bash
g++ -std=c++11 -o robot_knossos src/*.cpp
```

3. Run the game:
```bash
./robot_knossos [width] [height] [num_items]
```

### Usage Examples

```bash
# Minimum size maze with 5 items
./robot_knossos 16 16 5

# Medium maze with 12 items  
./robot_knossos 30 30 12

# Large maze with 25 items
./robot_knossos 50 50 25
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` | Move Up |
| `A` | Move Left |
| `S` | Move Down |
| `D` | Move Right |
| `Q` | Quit Game |
| `E` | Fix Corrupted Console |

## 📊 Performance Benchmarks

The game includes built-in performance monitoring for maze generation:

| Maze Size | Generation Time |
|-----------|----------------|
| 16x16     | ~631 μs       |
| 30x30     | ~1.55 ms      |
| 50x50     | ~5.12 ms      |
| 100x100   | ~17.75 ms     |

## 🏗️ Project Structure

```
robot-in-knossos/
├── src/
│   ├── main.cpp
│   ├── Matrix.cpp
│   ├── Gameplay.cpp
│   ├── MatrixField.cpp
│   ├── FileHandler.cpp
│   └── ConsoleHandler.cpp
├── include/
│   └── [header files]
├── tests/
│   └── [test files]
├── results/
│   └── [saved game states]
└── README.md
```

## 🎯 Game Objectives

- **Primary Goal**: Navigate the robot from entrance to exit
- **Survival**: Avoid or defeat the Minotaur
- **Strategy**: Collect and use items strategically
- **Efficiency**: Complete the maze in minimum moves

## 🎓 Academic Context

This project was developed as part of an Object-Oriented Programming course at the Faculty of Technical Sciences, University of Novi Sad. It demonstrates practical application of:

- Object-oriented design patterns
- Dynamic memory management
- Algorithm implementation and optimization
- User interface design in console applications
- Software testing methodologies

## 👨‍💻 Author

**Marko Sladojević**
- Student ID: SV33-2023
- Faculty of Technical Sciences, University of Novi Sad
- Course: Object-Oriented Programming 2
- Mentor: Dr. Dušan Kenjić

---

*Inspired by the legendary Labyrinth of Knossos and the myth of Theseus and the Minotaur* 🏛️⚡
