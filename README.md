REQUIREMENTS:
OS->LINUX
LIBRARY TO INSTALL->OPENSSL

HOW TO RUN PROJECT------------

STEP 1: RUN TRACKER
-> cd tracker
-> g++ tracker.cpp
-> ./a.out tracker_info.txt
tracker_info.txt contains IP and PORT in two separate lines on which tracker will run.
example :-
127.0.0.1
5500

STEP 2: RUN CLIENTS
-> cd ../client
-> g++ client.cpp
-> ./a.out <client_ip>:<client_port> tracker_info.txt
example :-
./a.out 127.0.0.1:5506 tracker_info.txt
TO RUN ONE MORE CLIENT WE CAN HAVE MULTIPLE IP AND PORT EXAMPLE :-
./a.out 127.0.0.4:18011 tracker_info.txt

STEP 3: OPERATIONS A CLIENT CAN PERFORM
a) create_user <user_name> <password>
   example :-
   "create_user u1 p1"
b) login <user_name> <password>
   example :-
   "login u1 p1"
c) logout
   example :-
   "logout"
d) create_group <group_name>
   example :-
   "create_group g1"
e) join_group <group_name>
   example :-
   "join_group g1"
f) list_requests <group_name>
   example :-
   "list_requests g1>
g) accept_requests <group_name> <user_name>
   example :-
   "acept_requests g1 u2"
h) list_groups
   example :-
   "list_groups"
i)  leave_group <group_name>
    example :-
    "leave_group g1"
j)  list_files <group_name>
    example :-
    "list_files g1"
k)  upload_file <filepath> <group_name>
    example :-
    "upload_file /home/anuja/Downloads/sample1.jpeg g1"
l)  download_file <group_name> <filename> <destination_path>
    example :-
    "download_file g1 sample1.jpeg /home/anuja/Pictures"
m)  show_downloads
    example :-
    "show_downloads"
n)  stop_share <group_name> <filename>
    example :-
    "stop_share g1 sample1.jpeg"
