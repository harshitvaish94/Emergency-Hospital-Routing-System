#  Smart Hospital Network System

A C-based emergency hospital routing and bed management system that intelligently assigns accident victims to the nearest available hospital using graph algorithms and data structures.

> Built as a **Data Structures & Algorithms project** with real-world inspiration.

---

##  Problem Statement

During medical emergencies, patients often face critical delays because:
- The nearest hospital may be fully occupied
- Ambulances lack real-time bed availability information
- There is no intelligent routing system to distribute patients efficiently

This leads to overcrowded hospitals, wasted travel time, and delayed treatment.

---

##  Our Solution

The **Smart Hospital Network System** solves this problem by:
- Maintaining real-time hospital bed availability
- Using **Dijkstra’s Algorithm** to find the nearest hospital
- Automatically skipping hospitals with no free beds
- Prioritizing critical patients using a **Priority Queue**

The system simulates how a real emergency control room could operate.

---

##  Key Features

- 🏥 **Hospital Bed Management**
  - Each hospital maintains a linked list of beds
  - Beds are dynamically allocated and released

-  **Shortest Path Routing**
  - Accident areas and hospitals are modeled as a weighted graph
  - Dijkstra’s algorithm finds the nearest hospital by distance

-  **Priority-Based Patient Handling**
  - Patients are handled based on severity:
    - Severity 1 → Critical
    - Severity 2 → High
    - Severity 3 → Normal

-  **Live Console Dashboard**
  - View hospital bed availability
  - View area-to-hospital distance mapping

-  **Persistent Storage**
  - Hospital and area data is stored in a text-based database
  - Updates are saved back to the file on exit

---

##  Tech Stack & Data Structures

| Component | Technology / DS Used |
|----------|----------------------|
| Programming Language | C |
| Routing Logic | Graph (Adjacency Matrix) |
| Shortest Path | Dijkstra’s Algorithm |
| Bed Management | Linked List |
| Emergency Queue | Priority Queue (Min-Heap) |
| Storage | File Handling (`database.txt`) |
| Memory Management | `malloc`, `calloc`, `free` |

---

##  Project Structure
Smart-Hospital-Network-System/
│
├── main.c # Main menu & program flow
├── smart_hospital.c # Core logic (graph, PQ, beds)
├── smart_hospital.h # Declarations & structures
├── database.txt # Hospitals, areas & distances
├── README.md # Project documentation
├── .gitignore
└── LICENSE

## How to Compile & Run

### Requirements
- GCC compiler
- Any C-compatible terminal (VS Code, CMD, Linux terminal)

### Compile

```bash
gcc main.c smart_hospital.c -o smart_hospital
```


Sample Scenario
Accident reported at: Indiranagar
Severity: Critical

→ Nearest hospital checked
→ If full, next nearest hospital selected
→ Bed allocated dynamically

Output:
Patient Harish admitted to CloudNine Indiranagar (Bed 4) — Distance: 9 km
Real-World Relevance 
Similar issues have been reported in real hospitals where accident victims were denied beds due to overcrowding.

This project demonstrates how algorithmic decision-making can improve emergency healthcare response.

FUTURE AI/ML SCOPE

- Predict hospital overcrowding using ML
- Emergency demand forecasting
- Intelligent rerouting using reinforcement learning
- Real-time ambulance GPS optimization


