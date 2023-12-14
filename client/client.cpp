#include <bits/stdc++.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
using namespace std;
vector<string>down;//downloaded files

string c_ip;
int c_port;

ofstream fout;
string line;

void fun2(int client_socket){
while(true){
char buffer[1024] = { 0 };
int valread;
valread = read(client_socket, buffer, 1024);
printf("%s\n",buffer);
string str(buffer);
string fp="",word;
stringstream ss(str);
int f1=0;
while(!ss.eof()){
f1++;
getline(ss,word,' ');
if(f1==3)fp=word;
}
printf("%s\n", buffer);

ifstream file(fp, ios::binary);
const string sd="openssl sha1 "+fp+" > output1.txt";
system(&sd[0]);
file.seekg(0, ios::end);
int size = file.tellg();
file.seekg(0, ios::beg);
char* buffer1 = new char[size];
file.read(buffer1, size);
file.close();
int* fsize = &size;
int err = send(client_socket, (char*)fsize, sizeof(int), 0);
if (err <= 0)
{
    printf("Error in sending \n");
}
err = send(client_socket, buffer1, size, 0);
if (err <= 0)
{
    printf("Error in sending \n");
}
delete[] buffer1;




/*ifstream fin;
fin.open(fp);
string content="";word="";
while(fin){
getline(fin,word);
content+=word;
}
//send(client_socket , &content[0], 1024);
send(client_socket, &content[0] ,strlen(&content[0]), 0);*/
file.close();

}
}

void* fun(void* arg){
int server_socket,op=1;
struct sockaddr_in address;
int addrlen = sizeof(address);
if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
perror("socket failed");
exit(EXIT_FAILURE);
}
if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &op, sizeof(op))) {
perror("setsockopt");
exit(EXIT_FAILURE);
}
address.sin_family = AF_INET;
address.sin_port = htons(c_port);

if(inet_pton(AF_INET, &c_ip[0], &address.sin_addr)<=0)  {
printf("\nInvalid address/ Address not supported \n");
return NULL;
}

if (bind(server_socket, (struct sockaddr*)&address,  sizeof(address))<0) {
perror("bind failed");
exit(EXIT_FAILURE);
}

if (listen(server_socket, 3) < 0) {
perror("listen");
exit(EXIT_FAILURE);
}

/*int new_socket;
if((new_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t *)&addrlen)) < 0){
perror("Acceptance error");
}
char buffer[1024] = { 0 };
int valread;
valread = read(new_socket, buffer, 1024);
printf("%s\n", buffer);
send(new_socket, "From peer as server", 38,0);
/*vector<thread> v;
while(true){
int new_socket;
if((new_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t *)&addrlen)) < 0){
perror("Acceptance error");
}
v.push_back(thread(file_request, new_socket));
}
for(auto it=v.begin(); it!=v.end();it++){
if(it->joinable()) it->join();
}*/
vector<thread> v;

while(true){
int new_socket;
if((new_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t *)&addrlen)) < 0){
perror("Acceptance error");
}
v.push_back(thread(fun2, new_socket));
}
for(auto i=v.begin(); i!=v.end(); i++){
if(i->joinable()) i->join();
}
}

int main(int argc,char *argv[]){

fout.open("../logfile1.txt",ios::app);

string s_ip;
int s_port;
string s=argv[1],word;
stringstream ss(s);
int f=0;
while(!ss.eof()){
f++;
getline(ss,word,':');
if(f==1)c_ip=word;
else c_port=stoi(word);
if(f==2)break;
}
//cout<<c_ip<<" "<<c_port<<endl;
fstream fp;//declare file pointer
fp.open(argv[2],ios_base::in);//opened in reading mode the tracker_file.txt
if(!fp.is_open()){
cout<<"Tracker_info.txt does not exists";
exit(1);
}
string t;
f=0;
while(getline(fp,t)){
f++;
if(f==1)s_ip=t;//ip address
if(f==2)s_port=stoi(t);//port
if(f==2)break;//we have read 2 lines containing ip and port no of tracker
}
//cout<<s_ip<<" "<<s_port;

int sock = 0, valread, client_fd;
struct sockaddr_in serv_addr;
char* hello = "Hello from client";
char buffer[1024] = { 0 };
if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
printf("\n Socket creation error \n");
return -1;
}

// here is the code for client acting as server
struct sockaddr_in s_socket;
pthread_t s_thread;
if(pthread_create(&s_thread, NULL, fun, NULL) == -1){
perror("pthread");
exit(1);
}

serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(s_port);
if (inet_pton(AF_INET, &s_ip[0], &serv_addr.sin_addr)<= 0) {
printf("\nInvalid address/ Address not supported \n");
return -1;
}
if ((client_fd= connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))< 0) {
printf("\nConnection Failed \n");
return -1;
}

line=c_ip+":"+to_string(c_port)+" connected to tracker ";
fout<<line<<endl;
fout.flush();

while(true){
cout << ">>> ";
string inp;
getline(cin, inp);
if(inp.length() < 1) continue;

string cmd,arg1,arg2,arg3,word;
int f=0;
stringstream ss4(inp);
while(!ss4.eof()){
f++;
getline(ss4,word,' ');
if(f==1)cmd=word;
else if(f==2)arg1=word;
else if(f==3)arg2=word;
else if(f==4)arg3=word;
//cout<<word<<endl;
//break;
}
//cout<<c_ip<<endl<<c_port<<endl;
inp+=" "+c_ip+" "+to_string(c_port);
//cout<<inp<<endl;


send(sock, &inp[0] ,strlen(&inp[0]), 0);
char server_reply[10240];
bzero(server_reply, 10240);
read(sock , server_reply, 10240);

if(cmd=="show_downloads"){
for(string i:down)cout<<"[C] "<<i<<endl;
}

//cout<<"-->"<<cmd<<endl;
else if(cmd=="download_file" ){
//string k="hello";
string str(server_reply);
stringstream ss2(str);
//ss(k);
string a,b,c;
int f=0;
while(!ss2.eof()){
f++;
getline(ss2,word,' ');
if(f==1)a=word;
if(f==4)c=word;
if(f==3)b=word;
}
string a1;
int a2;
string aa(a);
stringstream ss3(aa);
f=0;
while(!ss3.eof()){
f++;
getline(ss3,word,':');
if(f==1)a1=word;
else a2=stoi(word);
}
cout<<"Downloading!"<<endl;
cout<<"1"<<endl;
//cout<<a1<<" "<<a2<<" "<<b<<" "<<c<<endl;
cout<<"2"<<endl;
string sen=a1+" "+to_string(a2)+" "+b+" "+c;
//system("./a.out 127.0.0.1:5004 client_info.txt");
cout<<"3"<<endl;
int sock2 = 0, valread2, client_fd2;
struct sockaddr_in serv_addr2;
char buffer2[1024] = { 0 };
if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
printf("\n Socket creation error \n");
return -1;
}
cout<<"4"<<endl;

serv_addr2.sin_family = AF_INET;
//cout<<a1<<" "<<a2<<endl;
serv_addr2.sin_port = htons(a2);
//cout<<sen<<endl;
if (inet_pton(AF_INET,&a1[0], &serv_addr2.sin_addr)<= 0) {
printf("\nInvalid address/ Address not supported \n");
return -1;
}
cout<<"5"<<endl;
if ((client_fd2= connect(sock2, (struct sockaddr*)&serv_addr2,sizeof(serv_addr2)))< 0) {
printf("\nConnection Failed \n");
return -1;
}
cout<<"6"<<endl;
send(sock2, &sen[0] ,strlen(&sen[0]), 0);
/*char server_reply1[1024];
bzero(server_reply1, 1024);
read(sock2 , server_reply1, 1024);
cout<<server_reply1;*/

int filesize = 0;
int err = recv(sock2, (char*)&filesize, sizeof(filesize), 0);
if (err <= 0)
{
    printf("Error in sending \n");
}

char* buffer = new char[filesize];
err = recv(sock2, buffer, filesize, MSG_WAITALL);
if (err <= 0)
{
    printf("Error in sending \n");
}
string base_filename = b.substr(b.find_last_of("/\\") + 1);
c=arg3+"/"+base_filename;
ofstream file(c, ios::binary);
if(!file.is_open())cout<<"Error opening file!! "<<"with path "<<c<<endl;
file.write(buffer, filesize);
delete[] buffer;
file.close();

cout<<"7"<<endl;
/*fstream file;
string base_filename = b.substr(b.find_last_of("/\\") + 1);
c=c+"/"+base_filename;
file.open(c,ios::out);
file<<server_reply1;*/
cout<<"Downloaded!!"<<endl;
const string sd="openssl sha1 "+c+" > output2.txt";
system(&sd[0]);
ifstream infile1("output1.txt");
ifstream infile2("output2.txt");
string buffera1,buffera2;
while(getline(infile1,buffera1))
{
cout<<"Source file-->" << buffera1 << endl;
break;

}
while(getline(infile2,buffera2))
{
cout<<"Downloaded file-->" << buffera2 << endl;
break;
}
if(buffera1.substr(buffera1.find("="))==buffera2.substr(buffera2.find("=")))cout<<"SHA1 HASH MATCHED!!"<<endl;
else cout<<"ERROR IN DOWNLOADING!!"<<endl;

line="Downloaded file "+base_filename+" on "+c_ip+" : "+to_string(c_port);
fout<<line<<endl;
fout.flush();
down.push_back(base_filename);
}
//cout<<"hah ";
else cout << "from server/client --->  "<<server_reply << endl;
}
close(sock);
return 0;
}
