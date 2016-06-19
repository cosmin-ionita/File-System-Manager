# File-System-Manager

This project was created in order to simulate a real file - management system, over a network. Basically, there are 2 separate applications, a client and a server,
and the main idea is that the server maintains a manageable directory which contains the files of each client. The client can modify the state of it's directory on the server
using relevant commands (download, upload, share, unshare). All the files on the server are labeled as private or public and each client can download the public files
of any other client (it cannot modify those files).

The clients can transfer files (via download / upload commands) to (from) the server. There is a maximum bandwith on the connection line (set by design) and any file
with the size greater than that limit has to be split in chunks, transfered and then reassembled to create the initial file, exactly as TCP does. Many clients can
transfer files in the same time, so the server has to keep track of each chunk.

![FSM_Architecture](https://github.com/ionitacosmin95/File-System-Manager/blob/master/Images/file_system_manager.png)



