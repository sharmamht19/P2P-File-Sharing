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

### Software Requirements

- **G++ Compiler**:

  ```bash
  sudo apt-get install g++
  ```

- **OpenSSL Library**:

  ```bash
  sudo apt-get install openssl
  ```

- **Platform**: `Linux`

## Installation

  ```sh
    cd client
    make
    cd ../tracker
    make
  ```

## Usage

### Tracker

- Starting the Tracker:

  ```sh
  cd tracker
  ./tracker <TRACKER INFO FILE> <TRACKER NUMBER>
  ```

#### Example

  ```sh
  ./tracker tracker_info.txt 1
  ```

- Closing the Tracker:

  ```sh
  quit
  ```

### Client

1. ### Starting the Client

```sh
cd client
./client <IP>:<PORT> <TRACKER INFO FILE>
```

#### Example

```sh
./client 127.0.0.1:18000 tracker_info.txt
```

### 2. User Management Commands

- Create an account: `create_user <user_id> <password>`
- Log in: login `<user_id> <password>`
- Log out: `logout`

### 3. Group Operations

- Create a group: `create_group <group_id>`
- Join a group: `join_group <group_id>`
- Leave a group: `leave_group <group_id>`
- Accept a group join request: `accept_request <group_id> <user_id>`
- List all groups: `list_groups`
- List sharable files in a group: `list_files <group_id>`
- View join requests: `list_requests <group_id>`

### 4. File Management

- Upload: `upload_file <file_path> <group_id>`
- Download: `download_file <group_id> <file_name> <destination_path>`
- Stop sharing: `stop_share <group_id> <file_name>`
- View downloads: `show_downloads`

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
