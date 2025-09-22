# Trivia Server-Client Project

## Overview
This project is a fully-featured **Trivia System** that implements a **client-server architecture** using sockets.  
The system was designed from the ground up with a **custom stateful protocol**, ensuring persistent and secure interactions between the client and server.  
It demonstrates advanced concepts in **network programming, multithreading, database management, and protocol design**, while delivering a fun and interactive trivia experience powered by real-time questions.

---

## Features
- **Server-Client Communication with Sockets**  
  A robust communication layer that enables fast and reliable data transfer.

- **Custom Stateful Protocol**  
  A fully designed protocol that maintains user session state across interactions.

- **Authentication on Every User Action**  
  - Each client action requires authentication.  
  - Server-side errors are safely transferred to the client with **easy-to-understand contextual messages**.

- **SQLite Database Integration**  
  Stores user data securely and efficiently.  

- **Full Server Logging**  
  Tracks every action with detailed logs, including **file-writing** for persistence and auditing.

- **Thread-Based Server**  
  - A new thread is created on user login and destroyed on logout.  
  - Ensures concurrency and efficient use of server resources.

- **Trivia API Integration**  
  Fetches real trivia questions dynamically from [OpenTDB](https://opentdb.com/).

---

## Tech Stack
- **Language**: C++ (up to C++17)  
- **Database**: SQLite  
- **Networking**: TCP Sockets  
- **Concurrency**: Multithreading with thread creation and destruction per session  
- **External API**: [OpenTDB](https://opentdb.com/)

---

## Logging
- All server actions are logged with timestamps.
- Logs are saved to files for long-term analysis and debugging.

---

## Contact
For more details or inquiries:
- 📞 Phone: 0532782264
- 📧 Email: idoengel0707@gmail.com
- 🌐 GitHub: IdoEngel
