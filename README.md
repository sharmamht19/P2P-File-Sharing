# Peer-to-Peer Group Based File Sharing System

A robust peer-to-peer (P2P) file sharing system designed for efficient sharing and downloading within specific groups. The system uses SHA1 hashing to ensure file integrity and provides parallel downloading capabilities from multiple peers.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [Tracker](#tracker)
  - [Client](#client)
- [System Workflow](#system-workflow)
- [Assumptions & Limitations](#assumptions--limitations)

## Prerequisites

### Software Requirements:
- **G++ Compiler**
  ```bash
  sudo apt-get install g++
  ```
- **OpenSSL library**
  ```bash
  sudo apt-get install openssl
```

**Platform:** Linux

## Installation

```
1. cd client
2. make
3. cd ../tracker
5. make
6. cd ..
```

## Usage

### Tracker

1. Run Tracker:

```
cd tracker
./tracker​ <TRACKER INFO FILE> <TRACKER NUMBER>
ex: ./tracker tracker_info.txt 1
```

`<TRACKER INFO FILE>` contains the IP, Port details of all the trackers.

```
Ex:
127.0.0.1
5000
127.0.0.1
6000
```

2. Close Tracker:

```
quit
```

### Client:

1. Run Client:

```
cd client
./client​ <IP>:<PORT> <TRACKER INFO FILE>
ex: ./client 127.0.0.1:18000 tracker_info.txt
```

2. Create user account:

```
create_user​ <user_id> <password>
```

3. Login:

```
login​ <user_id> <password>
```

4. Create Group:

```
create_group​ <group_id>
```

5. Join Group:

```
join_group​ <group_id>
```

6. Leave Group:

```
leave_group​ <group_id>
```

7. List pending requests:

```
list_requests ​<group_id>
```

8. Accept Group Joining Request:

```
accept_request​ <group_id> <user_id>
```

9. List All Group In Network:

```
list_groups
```

10. List All sharable Files In Group:

```
list_files​ <group_id>
```

11. Upload File:

```
​upload_file​ <file_path> <group_id​>
```

12. Download File:​

```
download_file​ <group_id> <file_name> <destination_path>
```

13. Logout:​

```
logout
```

14. Show_downloads: ​

```
show_downloads
```

15. Stop sharing: ​

```
stop_share ​<group_id> <file_name>
```

## System Workflow
- Users register and log in.
- Tracker maintains client details.
- Create/join groups.
- Group admins manage join requests.
- Share files across groups, notifying the tracker.
- List all sharable files within groups.
- Download files from multiple peers.
- SHA1 ensures file integrity during transfers.

## Assumptions & Limitations
- A single always-online tracker is implemented.
- Peers can use different IPs, but session data isn't persistent.
- Potential SHA1 inconsistency with binary files.
- Use absolute file paths.

