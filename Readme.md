# 📁 Snapdrop - Peer-to-Peer File Sharing over LAN  

**Snapdrop** is a CLI-based peer-to-peer file-sharing application for local networks. It allows users to send and receive files between devices without requiring a central server.  

## 🚀 Features  

- 📡 **Peer Discovery** - Automatically detects other devices on the network using udp broadcast.  
- 🔄 **Direct File Transfer** - Send files directly between peers using TCP.  
- 📤 **Send Files** - Easily send files to a specified peer.  
- 📥 **Receive Files** - Accept incoming file transfers.  
- 🏷️ **Preserves File Name** - The original file name is retained upon transfer.  
- 📊 **Progress Bar** - Displays real-time progress while sending and receiving files.  
- 🔀 **Multiple Clients Support** - Uses multithreading to handle multiple transfers at once.  
- 🔧 **Lightweight & Fast** - Built using C++ with minimal dependencies.  
- 💬 **Send Messages** - Send message directly between peers.
- **Multiple file transfer** - Send multiple files together. Also allows sending folder in zip form.
---

## 🛠️ Installation  

### 1️⃣ **Build from Source**  

#### Prerequisites  
- C++17 or later  
- GNU Make  
- g++ (GCC)  
- CMake (Optional)  

#### Steps  
```sh
git clone https://github.com/Zoro-369/FileShare.git
cd FileShare
make
```

### 2️⃣ **Run with Docker**  

#### Build Docker Image  
```sh
docker build -t snapdrop .
```
#### Creating Custom network inside docker
```sh
docker network create snapdrop-net

# Run First Instance
docker run --rm -it --network=snapdrop-net --name peer1 snapdrop

# Run Second Instance
docker run --rm -it --network=snapdrop-net --name peer2 snapdrop

# Removing Custom Network
docker network rm snapdrop-net 
```

---

## 📌 Usage  

### 🕵️ Discover Peers  
```sh
./snapdrop -d
```

### 📤 Send a File  
```sh
./snapdrop -s <peer_ip> <file1> <file2> ...
```

Example:  
```sh
./snapdrop -s 192.168.1.100 example.txt
```

### 📥 Receive Files  
```sh
./snapdrop -r
```

### 💬 Send Messages
```sh
./snapdrop -m <peer_ip> <message>
```
### ❓ Help  
```sh
./snapdrop -h
```

---

## 📂 Project Structure  

```
snapdrop  
├── include  
│   ├── FileReceiver.h  
│   ├── FileSender.h  
│   ├── PeerDiscovery.h  
│   ├── utils.h  
├── src  
│   ├── FileReceiver.cpp  
│   ├── FileSender.cpp  
│   ├── PeerDiscovery.cpp  
│   ├── main.cpp  
│   ├── utils.cpp  
├── build/ (Generated)
├── received_files  
├── Dockerfile  
├── Makefile  
├── README.md  
```

---

## ⚙️ How It Works  

1️⃣ Devices on the same network discover each other via UDP broadcast.  
2️⃣ A sender initiates a file transfer request.  
3️⃣ The receiver accepts the request and receives the file over TCP.  
4️⃣ The file is stored with its original name in the designated directory.  
5️⃣ A **progress bar** shows real-time transfer updates.  
6️⃣ The receiver supports **multiple clients simultaneously** using **multithreading**.  

---

## 📊 Progress Bar Example  

**Sending File:**  
```
Sending file: example.txt (1024 KB)
[##########----------] 50% Complete
```

**Receiving File:**  
```
Receiving file: example.txt (1024 KB)
[####################] 100% Complete
File received successfully.
```

---

## 🛠️ Troubleshooting  

🔹 **File Transfer Fails?**  
- Ensure both sender & receiver are on the same network.  
- Check firewall settings (allow traffic on required ports).  

🔹 **Docker Issues?**  
- Ensure volumes (`-v`) are correctly mapped.  
- Run the container with `--network=host` for better connectivity.  

---
