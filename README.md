# P2P File Sharing System

- **Developed a robust P2P file sharing system** enabling efficient sharing, downloading, and management of files within specific user groups, inspired by BitTorrent protocols.
  
- **Leveraged multithreading** to allow concurrent operations, improving responsiveness and throughput of the system.

- **Utilized socket programming** to establish peer-to-peer and client-server communication channels, enabling real-time data exchange and synchronization.

- **Implemented SHA1 hashing** to validate file integrity, ensuring data reliability and security during transfers.

- **Introduced parallel downloading functionality**, allowing users to simultaneously retrieve multiple file pieces from different peers, expediting the download process.

- **Architected a system with a central tracker** responsible for metadata storage; the tracker keeps track of which users have which files, streamlining the discovery process for clients.

- **Incorporated advanced file distribution algorithms** including:
  - `Random Piece Selection`
  - `Rarest Piece Selection`

## Table of Contents

- [Functional Requirements](#functional-requirements)
- [Non-functional Requirements](#non-functional-requirements)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [Tracker](#tracker)
  - [Client](#client)

- [File Piece Selection Algorithms](#file-piece-selection-algorithms)
  - [Random Piece Selection](#1-random-piece-selection)
  - [Rarest Piece Selection](#2-rarest-piece-selection)
- [Data Integrity with SHA-1](#data-integrity-with-sha-1)

- [System Workflow](#system-workflow)
- [Assumptions & Limitations](#assumptions--limitations)
- [Contributing](#contributing)
- [License](#license)

## Functional Requirements

- **User Authentication**: Ability to register, login, and logout.
- **Group Management**: Users can create, join, or leave groups. Group admins have authority over join requests.
- **File Management**: Users can share files across groups, download from multiple peers, and halt sharing of specific files.
- **File Integrity**: Incorporation of SHA1 hashing to ensure file reliability during transfers.
- **File Discovery**: Centralized tracker to aid in the discovery of shared files across users and groups.
- **Parallel Downloading**: Simultaneous retrieval of multiple file pieces from diverse peers.
  
## Non-functional Requirements

- **Performance**: Rapid parallel downloading from multiple peers to expedite file transfers.
- **Reliability**: Ensured file integrity through SHA1 hashing.
- **Usability**: Clear command-line prompts and guides for user actions.
- **Security**: Secure file transfers and user authentication.

## Prerequisites

### Software Requirements

- **G++ Compiler**:

  ```sh
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

### 1. Starting the Client

```bash
cd client
./client <IP>:<PORT> <TRACKER INFO FILE>
```

### Example

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

## File Piece Selection Algorithms

### 1. Random Piece Selection

```pseudo
function getRandomPieceForSeeder(seeders, totalChunks):
    assignList = initializeEmptyListOfSize(totalChunks)

    for chunkNumber from 1 to totalChunks:
        randomSeeder = selectRandomFrom(seeders)
        while randomSeeder does not have chunkNumber:
            randomSeeder = selectRandomFrom(seeders)
        
        assignList[chunkNumber] = randomSeeder
    return assignList
```

Random Piece Selection is a strategy we use primarily during the early phases of a file download to boost efficiency. In this method, a client randomly selects a piece of the file from the available seeders. By doing this, clients diversify the pieces they're downloading, preventing everyone from requesting the same pieces simultaneously. This method maximizes bandwidth usage, reduces download time, and ensures a uniform spread of the file across peers.

### 2. Rarest Piece Selection

```pseudo
function getRarestPieceForSeeder(seeders, totalChunks):
    pieceFrequency = initializeArrayOfSize(totalChunks, with value 0)
    assignList = initializeEmptyListOfSize(totalChunks)

    for each seeder in seeders:
        for each chunk that seeder has:
            increment pieceFrequency[chunk]

    for chunkNumber from 1 to totalChunks:
        seederForChunk = getSeederWithLeastFrequencyForChunk(seeders, pieceFrequency)
        assignList[chunkNumber] = seederForChunk

    return assignList
```
  
The Rarest Piece Selection algorithm ensures optimal distribution of file pieces within the network. It prioritizes the download of the rarest pieces first, which means that clients will opt to download pieces that are the least available amongst the seeders. This strategy ensures that scarce file pieces are propagated faster across the network, making it more resilient and efficient. If a seeder with a rare piece were to leave the network, the chances of that piece being available with another peer are higher, thanks to this approach.

## Data Integrity with SHA-1

In our system, we prioritize data integrity by utilizing the SHA-1 hash algorithm on our data chunks. Here's a simple breakdown:

1. **Read the File in Chunks**: The file is read in portions, each of size `CHUNK_SIZE`.
2. **Hash the Chunk**: For each chunk read, we compute its SHA-1 hash.
3. **Convert to Hexadecimal**: The resulting hash is then transformed into a hexadecimal string.

```pseudo
while bytesRead = readFileInChunks(buffer, CHUNK_SIZE) {
    hash = SHA1(buffer, bytesRead)
    hexStr = convertToHex(hash)
    shaList.append(hexStr)
}
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

## Contributing

Contributions/Suggestions are welcome!

## License

MIT License.
