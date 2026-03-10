# EasyCharge - EV Charging Station Management System

## Overview

This project is a comprehensive management system for **EasyCharge**, a national electric vehicle (EV) charging network. It was developed as a final project for the Advanced C course in the Electrical and Computer Engineering departments.

The system is written entirely in C and uses Abstract Data Types (ADTs) to efficiently manage charging stations, individual charging ports, registered customers, and waiting queues. It features robust file I/O operations, loading states upon startup and saving updates upon exit to maintain data normalization.

## Architecture & Data Structures

The project strictly avoids global variables and utilizes a highly modular architecture. The core data structures include:

- **Stations Database (`Stations BST`)**: A Binary Search Tree organizing all charging stations by a unique ID.
- **Cars Database (`Cars BST`)**: A Binary Search Tree organizing all registered vehicles by their unique 9-digit license plate number.
- **Ports List**: A singly linked list within each station node, managing the station's available charging ports (FAST, MID, SLOW).
- **Line of Cars (`Cars Queue`)**: A Queue implemented at each station to manage vehicles waiting for an available charging port.

## Technical Implementation Highlights

This project showcases advanced C programming techniques, prioritizing modularity, memory safety, and code reusability:

- **Generic Data Structures (Polymorphism in C):** Implemented a fully modular Binary Search Tree (`BinaryTree.c`) utilizing `void*` data payloads and custom function pointers (`CompareFunc`, `PrintFunc`, `FreeFunc`). This architectural choice allowed the exact same tree logic to seamlessly manage entirely different data entities (the Stations database and the Cars database) without code duplication.
- **Sophisticated File Loading Engine:** Designed a highly generic and robust file-parsing mechanism (`SystemData.c`). By using a `FileLoaderConfig` struct equipped with callback functions for parsing (`ParseLineFunc`) and linking data (`PostProcessFunction`), a single unified engine safely handles all file I/O operations across different formats. It includes built-in header skipping, buffer overflow protection, and dynamic memory allocation tracking.
- **Safe Memory Management:** Strict adherence to memory safety protocols, ensuring all dynamically allocated nodes, data structures, and strings are properly freed upon system exit or during node deletion routines.

## Key Features

The system operates via an interactive command-line menu offering the following capabilities:

- **Locate Nearest Station**: Finds the closest charging station based on user coordinates (x, y).
- **Charge Car**: Connects a vehicle to an appropriate port or adds it to the station's waiting queue if all ports are occupied.
- **Check Car Status**: Reports whether a specific car is currently charging or its position in the queue[cite: 399, 401, 403].
- **Stop Charge**: Disconnects a vehicle, calculates the payment based on charging duration, and advances the queue.
- **Station Statistics Report**: Generates reports on port utilization percentage, hardware failure rates, and relative station load.
- **Top Customers**: Displays the 5 vehicles with the highest total payments.
- **Port Management**: Allows administrators to add new ports to a station or remove out-of-order ports.
- **Auto-Release Ports**: Automatically disconnects vehicles that have been charging for over 10 hours.
- **Station Management**: View all stations, display cars at a specific station, or completely close down a station.

## File Management

Data is persistently stored and loaded from external text files:

- `Stations.txt`: Current stations in the system.
- `Ports.txt`: Updated data of charging ports associated with each station.
- `Cars.txt`: Registered vehicle database.
- `LineOfCars.txt`: The active queues of cars waiting at various stations.

## Compilation and Execution

This project uses **CMake** for cross-platform build automation. The project maintains a clean directory structure, separating source files (`src/`) and headers (`headers/`), and the CMake configuration automatically handles moving the `data/` directory to the build environment.

### Prerequisites

- CMake (version 3.10 or higher)
- A standard C compiler (GCC, Clang, or MSVC)

### Build Instructions

1. Clone the repository and navigate into it:
   ```bash
   git clone [https://github.com/ronrk/finalProject.git](https://github.com/ronrk/finalProject.git)
   cd finalProject
   ```
