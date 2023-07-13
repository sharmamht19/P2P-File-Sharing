# Peer-to-Peer Group Based File Sharing System

## Prerequisites

**Software Requirement**

- G++ compiler
  - To install G++: `sudo apt-get install g++`
- OpenSSL library
  - To install OpenSSL library: `sudo apt-get install openssl`

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


## Working

1. User should create an account and register with the tracker.
2. Login using the user credentials.
3. The tracker maintains information of clients and their shared files to assist in communication between peers.
4. User can create a Group and become the admin of that Group.
5. User can fetch a list of all Groups in the server.
6. User can join/leave a Group.
7. Group admin can accept Group joining requests.
8. Share a file across a Group: Shares the filename and SHA1 hash of the complete file, as well as piecewise SHA1 hashes, with the tracker.
9. Fetch a list of all sharable files in a Group.
10. Download a file:
 1. Retrieve peer information for the file from the tracker.
 2. Download the file from multiple peers simultaneously using a piece selection algorithm.
 3. All downloaded files will be shareable to other users in the same Group. File integrity is ensured using SHA1 comparison.
11. Piece selection algorithm: Selects a random piece and downloads it from a random peer that has that piece.
12. Show ongoing downloads.
13. Stop sharing a file.
14. Logout, which stops sharing all files.
15. When a client logs in, all previously shared files before logout will automatically be in sharing mode.

## Assumptions

1. Only one tracker is implemented, and it should always be online.
2. The peer can log in from different IP addresses, but the details of their downloads/uploads will not persist across sessions.
3. SHA1 integrity checking may not work correctly for binary files, although the downloaded files are likely to be correct.
4. File paths should be absolute.
