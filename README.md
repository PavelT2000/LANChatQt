# LANChatQt

![Qt](https://img.shields.io/badge/Qt-5.15%2B%20%7C%206.x-41CD52?logo=qt&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![Build](https://img.shields.io/badge/Build-qmake-blue)
![CI](https://img.shields.io/badge/CI-GitHub%20Actions-2088FF)

## 📖 Overview
**LANChatQt** is a lightweight, serverless LAN messaging application built with **C++17** and the **Qt Framework**. Designed for local network environments, it enables real-time text communication without requiring external servers or complex configuration. The application leverages a hybrid UDP/TCP networking model for automatic peer discovery and reliable message delivery.

## ✨ Key Features
- 🔍 **Zero-Config Discovery**: Automatic peer detection via UDP broadcast on startup
- 📡 **Reliable Messaging**: Point-to-point TCP connections for guaranteed message delivery
- 💓 **Presence Tracking**: Heartbeat mechanism with automatic timeout handling (~15s)
- 🖥️ **Real-Time UI**: Dynamic peer list with status indicators and live message rendering
- 🌐 **Cross-Platform**: Built on Qt, compatible with Windows, Linux, and macOS
- 🛡️ **Modern C++**: C++17 compliant with Qt 5.15+/6.x networking APIs

## 🏗️ Architecture
The project follows a modular, component-based architecture:

| Component | Responsibility |
|-----------|----------------|
| `MainWindow` | Qt UI layer. Handles user input, message display, and peer list rendering. |
| `ChatEngine` | Core business logic. Manages peer state, message routing, heartbeat timers, and network events. |
| `NetworkManager` | Networking abstraction. Handles UDP sockets (discovery/broadcast) and TCP sockets (messaging). |
| `Packet` | Serialization layer. Structures network payloads using `QDataStream` for type-safe transmission. |

## 📦 Prerequisites
- **Qt Framework**: 5.15+ or 6.x (requires `widgets` and `network` modules)
- **Compiler**: C++17 compliant (MSVC 2019+, GCC 9+, Clang 10+)
- **Build System**: `qmake` (included with Qt) or Qt Creator IDE
- **Network**: IPv4 LAN environment with UDP broadcast enabled

## 🛠️ Build & Installation
### Using Qt Creator (Recommended)
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/LANChatQt.git
   cd LANChatQt
   ```
2. Open `LANChatQt.pro` in Qt Creator.
3. Configure the kit (Desktop Qt 5.15/6.x MinGW/MSVC/Clang).
4. Click **Build** → **Run**.

### Command Line
```bash
qmake LANChatQt.pro
make -j$(nproc)  # Linux/macOS
# or nmake / jom on Windows
./LANChatQt      # Execute binary
```

## 🚀 Usage
1. **Launch** the application on multiple devices connected to the same LAN.
2. **Enter Display Name**: Type your desired username in the top text field. The chat engine initializes automatically on the first keystroke.
3. **Peer Discovery**: Other instances will appear in the peer list within ~5 seconds. Status indicators:
   - `●` Online/Active
   - `○` Inactive/Timeout pending
4. **Send Messages**: Type in the bottom input field and press `Enter`.
5. **View Chat**: Messages appear in the central browser with sender formatting.

> 💡 *Note: The `Send` button is currently reserved for future UI enhancements. Press `Enter` to transmit messages.*

## 🌐 Network Protocol & Design
### Port Configuration
- **Default Port**: `12345` (UDP & TCP)
- **Scope**: Local network only (IPv4)

### Communication Flow
| Protocol | Purpose | Packet Type |
|----------|---------|-------------|
| **UDP Broadcast** | Peer discovery & heartbeat | `ALIVE` |
| **TCP Unicast** | Reliable message delivery | `MESSAGE` |

### Packet Structure
Serialized via `QDataStream` (`Qt_6_0` version):
```
[MessageType (qint8)] [SenderName (QString)] [Content (QString)]
```
- `MessageType::ALIVE (0)`: Broadcast every 5s. Resets peer `liveStatus`.
- `MessageType::MESSAGE (2)`: Sent via established TCP connection.
- `MessageType::DEACTIVE (1)`: Reserved for graceful disconnects.

### Heartbeat & Timeout Logic
- Broadcast interval: `5000ms`
- Timeout threshold: `liveStatus > 2` (≈15s without heartbeat)
- Peers exceeding threshold are automatically removed and TCP connections are closed.

## 📁 Project Structure
```
LANChatQt/
├── .github/workflows/      # CI/CD pipeline (Windows build)
├── LANChatQt.pro           # qmake project configuration
├── main.cpp                # Application entry point
├── mainwindow.ui           # Qt Designer UI layout
├── mainwindow.h/cpp        # UI controller & event handlers
├── chatengine.h/cpp        # Core logic, peer management, timers
├── networkmanager.h/cpp    # UDP/TCP socket abstraction
├── packet.h/cpp            # Network payload serialization
└── .gitignore / .aiignore  # Version control & AI context filters
```

## 🤝 Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License
This project is licensed under the [MIT License](LICENSE). See the `LICENSE` file for details.

---
*Built with ❤️ using C++17 & Qt* 🚀