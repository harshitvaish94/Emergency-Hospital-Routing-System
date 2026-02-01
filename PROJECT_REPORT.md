## 1. Problem Statement

During medical emergencies such as road accidents, patients are usually taken to the nearest hospital. However, in many real-life situations, the nearest hospital may already be full or lack the required bed capacity. This results in:

- Delays in treatment  
- Overcrowding of certain hospitals  
- Wasted travel time for ambulances  
- Increased risk to critical patients  

There is no simple system that simultaneously considers **distance**, **bed availability**, and **patient priority** while routing emergency patients.

---

## 2. Motivation & Real-World Relevance

Several real-life incidents have highlighted this issue:

- Accident victims being denied beds due to overcrowded emergency wards  
- Patients forced to wait or lie on the floor because no beds were available  
- Ambulances travelling to multiple hospitals before finding availability  

These situations demonstrate the need for an **intelligent hospital routing system** that can guide patients to the **nearest available hospital**, instead of just the nearest hospital.

This project aims to simulate such a system using **Data Structures and Algorithms**.

---

## 3. Proposed Solution

The **Smart Hospital Network System** provides a solution by:

- Maintaining real-time hospital bed availability  
- Modeling accident areas and hospitals as a **graph**  
- Using **Dijkstra’s Algorithm** to find the shortest distance hospital  
- Skipping hospitals that are full  
- Prioritizing critical patients using a **priority queue**

The system ensures faster admission and better distribution of patients across hospitals.

---

## 4. Data Structures Used

### 4.1 Graph (Adjacency Matrix)

- Accident areas and hospitals are represented as **nodes**
- Distances between areas and hospitals are represented as **edge weights**
- Stored using an **adjacency matrix**

**Purpose:**  
To compute the shortest distance from an accident area to all hospitals.

---

### 4.2 Linked List (Hospital Bed Management)

- Each hospital maintains a **linked list of beds**
- Each node represents a bed with:
  - Bed ID
  - Occupied / Free status

**Purpose:**  
To dynamically allocate and manage hospital beds efficiently.

---

### 4.3 Priority Queue (Patient Handling)

- Patients are inserted into a **priority queue**
- Priority is based on severity:
  - Severity 1 → Critical
  - Severity 2 → High
  - Severity 3 → Normal

**Purpose:**  
To ensure critical patients are admitted before less severe cases.

---

## 5. Algorithm Used – Dijkstra’s Algorithm

### Why Dijkstra?

Dijkstra’s Algorithm is used to find the **shortest path** in a weighted graph from a given source.

### How it is used in this project:

1. Accident area is treated as the **source node**
2. Dijkstra’s algorithm computes distances to all hospital nodes
3. Hospitals are checked in increasing order of distance
4. The nearest hospital with **at least one free bed** is selected

This ensures:
- Minimum travel distance
- Avoidance of full hospitals
- Optimal routing decision

---

## 6. Sample Execution

### Scenario:
- Accident Location: **Indiranagar**
- Patient Severity: **Critical**

### Process:
1. System checks distances from Indiranagar to all hospitals
2. Nearest hospital is evaluated
3. If beds are available → patient admitted
4. If full → next nearest hospital selected automatically

### Output:
Patient Harish admitted to CloudNine Indiranagar (Bed 4) — Distance: 9 km


---

## 7. Advantages of the System

- Reduces emergency response time
- Prevents hospital overcrowding
- Ensures fair distribution of patients
- Handles critical cases with priority
- Simple and efficient implementation using core DSA concepts

---

## 8. Limitations

- Data is static (file-based, not real-time)
- Does not include traffic or ambulance availability
- Console-based interface only

---

## 9. Future Scope

This project can be extended further by:

- Integrating real-time GPS and traffic data  
- Using IoT sensors for live bed updates  
- Adding a web or mobile dashboard  
- Applying machine learning to predict hospital load  
- Implementing city-wide or multi-city scalability  

---

## 10. Conclusion

The Smart Hospital Network System demonstrates how **Data Structures and Algorithms** can be applied to solve real-world healthcare problems. By combining graphs, Dijkstra’s algorithm, linked lists, and priority queues, the system efficiently routes emergency patients to the nearest available hospital and improves emergency response management.

---
