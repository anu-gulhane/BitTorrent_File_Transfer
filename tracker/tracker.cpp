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
#include<bits/stdc++.h>
using namespace std;


map<string,string>users;//to store user credentials
map<string,int>loggedin;//to store which users are currently logged in
map<string,string>admin;//group-name to user-name map
map<string,vector<string>>groups;//group-name to list of user-names map
map<string,vector<string>>requests;//group-name to list of user-names request to join group
map<string,vector<string>>files;//group-name to list of files shared in that group
map<string,string>path;//file-name to filename-path pairs
map<string,string>ipport;//user-name to ipport
map<string,string>uploadby;//file-name to user-name

ofstream fout;
string line;

void* fun(void* arg){
while(true){
string inputline;
getline(cin, inputline);
if(inputline == "quit"){
exit(0);
}
}
}
void* commands(int new_socket){

string uid,upass;
while(true){
int valread;
char buffer[1024] = { 0 };
valread = read(new_socket, buffer, 1024);
stringstream ss(buffer);
string cmd,arg1,arg2,arg3,word,arg4,arg5;
int f=0;
while(!ss.eof()){
f++;
getline(ss,word,' ');
if(f==1)cmd=word;
else if(f==2)arg1=word;
else if(f==3)arg2=word;
else if(f==4)arg3=word;
else if(f==5)arg4=word;
else if(f==6)arg5=word;
//cout<<word<<endl;
//break;
}

if(cmd=="stop_share"){
if(arg1==""||arg2=="")write(new_socket, "No. of arguments don't match", 38);
else if(admin.find(arg1)==admin.end())write(new_socket, "Group does not exist", 38);
else{
vector<string>temp=files[arg1];
vector<string>::iterator it;
for(it=temp.begin();it!=temp.end();it++){
if(*it==arg2)break;
}
temp.erase(it);
files[arg1]=temp;
string u="Stopped sharing "+arg2;
line=new_socket+" client "+u;
fout<<line<<endl;
fout.flush();
write(new_socket, &u[0], 38);
}
}

if(cmd=="create_user"){
if(arg1==""||arg2=="")write(new_socket, "No. of arguments don't match", 38);
else if(users.find(arg1)!=users.end())write(new_socket, "User with given name already exists!!", 38);
else {
users[arg1]=arg2;
line=new_socket+" client created user "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "User created in the database!", 38);
}
}

else if(cmd=="upload_file"){
string filePath = arg1;
struct stat buffer;
if(arg1==""|| arg2=="")write(new_socket, "No. of arguments don't match", 38);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(admin.find(arg2)==admin.end())write(new_socket, "Group does not exist", 38);
else if (stat(filePath.c_str(), &buffer) != 0)
{
write(new_socket, "File does not exist", 38);
}
else{
int i;
for (i = filePath.length() - 1; i >= 0; i--)
{
if (filePath[i] == '/')
{
break;
}
}
string fileName = filePath.substr(i + 1, filePath.length() - i - 1);
vector<string>v=files[arg2];
vector<string>::iterator it=find(v.begin(),v.end(),fileName);
if(it!=v.end())write(new_socket, "File already exists!!", 38);
else
{
v.push_back(fileName);
files[arg2]=v;
//vector<string>temp;
//temp.push_back(fileName);
//temp.push_back(filePath);
//vector<vector<string>>pairs;
//pairs.push_back(temp);
path[fileName]=arg1;
uploadby[fileName]=uid;
string ipport2=arg3+":"+arg4;
ipport[uid]=ipport2;
line=new_socket+" client uploaded file "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "File uploaded successfully!!", 38);
}
}
}


else if(cmd=="download_file"){

string dPath = arg3+"/"+arg2;
struct stat buffer;
if(arg1==""|| arg2==""||arg3=="")write(new_socket, "No. of arguments don't match", 38);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(admin.find(arg1)==admin.end())write(new_socket, "Group does not exist", 38);
else if (stat(dPath.c_str(), &buffer) == 0)
{
write(new_socket, "File already exists at destination", 38);
}
else {
vector<string>temp=files[arg1];
vector<string>::iterator it2=find(temp.begin(),temp.end(),arg2);
if(it2==temp.end())write(new_socket, "File does not exist", 38);
else{


string u="";
map<string,string>::iterator i;
for(i=uploadby.begin();i!=uploadby.end();i++){
if(i->first==arg2){
u=i->second;
break;
}
}

string ipaddr="";
map<string,string>::iterator i2;
for(i2=ipport.begin();i2!=ipport.end();i++){
if(i2->first==u){
ipaddr=i2->second;
break;
}
}

string fpath="";
map<string,string>::iterator i3;
for(i3=path.begin();i3!=path.end();i++){
if(i3->first==arg2){
fpath=i3->second;
break;
}
}

string ipport2=ipaddr;
ipport2+=" "+arg1+" ";


ipport2+=fpath;
ipport2+=" "+arg3+" "+u;;
write(new_socket, &ipport2[0], 68);
}
}
}

else if(cmd=="list_files"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(admin.find(arg1)==admin.end())write(new_socket, "Group does not exist", 38);
else{
int f=0;
vector<string>v=groups[arg1];
for(auto i:v){
if(i==uid){
f=1;
break;
}
}
if(f==0)write(new_socket, "You aren't part of this group!!", 58);
vector<string>v1=files[arg1];
string r1="Files in group: ";
for(auto i:v1)r1+=i+" ";
line=new_socket+" client asked to list files in group "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, &r1[0], 58);
}
}

else if(cmd=="login"){
if(arg1==""||arg2=="")write(new_socket, "No. of arguments don't match", 38);
else if(users.find(arg1)==users.end())write(new_socket, "Wrong User name/password!!", 38);
else {
if(loggedin.find(arg1)!=loggedin.end() && loggedin[arg1]==1)write(new_socket, "Already logged in!", 38);
else{
loggedin[arg1]=1;
uid=arg1;
upass=arg2;
line=new_socket+" client logged in "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "User successfully logged in!!", 38);
}
}
}
else if(cmd=="logout"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
else if(loggedin.find(arg1)==loggedin.end())write(new_socket, "Wrong User name!!", 38);
else{
if(loggedin[arg1]==0)write(new_socket, "Already logged out!", 38);
else{
loggedin[arg1]=0;
uid="";
upass="";
line=new_socket+" client logged out "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "User successfully logged out!!", 38);
}
}
}
else if(cmd=="create_group"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
if(admin.find(arg1)!=admin.end())write(new_socket, "Group-name already exists!!", 38);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else{
admin[arg1]=uid;
vector<string>v;
v.push_back(uid);
groups[arg1]=v;
vector<string>temp;
requests[arg1]=temp;
line=new_socket+" client created group "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "Group successfully created with you as admin!!", 48);
}
}
else if(cmd=="join_group"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
vector<string>v=groups[arg1];
vector<string>::iterator it=find(v.begin(),v.end(),uid);
if(admin.find(arg1)==admin.end())write(new_socket, "Group-name does not exists!!", 58);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(it!=v.end())write(new_socket, "You are already part of this group!!", 58);
else{
vector<string>temp=requests[arg1];
temp.push_back(uid);
requests[arg1]=temp;
line=new_socket+" client asked to join group "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "Your request to join group has been sent!!", 58);
}
}

else if(cmd=="leave_group"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
vector<string>v=groups[arg1];
vector<string>::iterator it=find(v.begin(),v.end(),uid);
if(admin.find(arg1)==admin.end())write(new_socket, "Group-name does not exists!!", 58);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(it==v.end())write(new_socket, "You are not part of this group!!", 58);
else if(admin[arg1]==uid)write(new_socket, "You are admin of this group, cant't leave!!", 58);
else{
vector<string>temp2=groups[arg1];
vector<string>::iterator it;
for(auto it=temp2.begin();it!=temp2.end();it++){
if(*it==uid){
temp2.erase(it);
groups[arg1]=temp2;
break;
}
}
line=new_socket+" client asked to leave group "+arg1 ;
fout<<line<<endl;
fout.flush();
write(new_socket, "User has been removed from this group!!", 58);
}
}

else if(cmd=="list_request"){
if(arg1=="")write(new_socket, "No. of arguments don't match", 38);
if(admin.find(arg1)==admin.end())write(new_socket, "Group-name does not exists!!", 58);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
int f=0;
map<string,string>::iterator it=admin.begin();
for(auto it=admin.begin();it!=admin.end();it++){
if(it->first==arg1 && it->second==uid){
f=1;
break;
}
}
if(f==0)write(new_socket, "You aren't the admin of this group!!", 58);
else{
vector<string>v=requests[arg1];
//write(new_socket, "Request by User:", 58);
string r1="Request by User: ";
//write(new_socket, &r1[0], 58);
for(auto i:v)r1+=i+" ";
write(new_socket, &r1[0], 58);
}
line=new_socket+" client asked to list files in group "+arg1;
fout<<line<<endl;
fout.flush();
}

else if(cmd=="accept_request"){
if(arg1==""|| arg2=="")write(new_socket, "No. of arguments don't match", 38);
else if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
else if(admin.find(arg1)==admin.end())write(new_socket, "Group does not exist", 38);
else if(users.find(arg2)==users.end())write(new_socket, "User does not exist", 38);
else{
map<string,string>::iterator it=admin.begin();
for(auto it=admin.begin();it!=admin.end();it++){
if(it->first==arg1 && it->second==uid){
f=1;
break;
}
}
if(f==0)write(new_socket, "You aren't the admin of this group!!", 58);
else{
vector<string>v1=groups[arg1];
vector<string>::iterator it=find(v1.begin(),v1.end(),arg2);
if(it!=v1.end())write(new_socket, "User is already part of this group!!", 58);
else{
vector<string>temp=groups[arg1];
temp.push_back(arg2);
groups[arg1]=temp;
vector<string>temp2=requests[arg1];
vector<string>::iterator it;
for(auto it=temp2.begin();it!=temp2.end();it++){
if(*it==arg2){
temp2.erase(it);
requests[arg1]=temp2;
break;
}
}
line=new_socket+" client accepted request of user "+arg2+" to join group "+arg1;
fout<<line<<endl;
fout.flush();
write(new_socket, "User added to this group!!", 58);
}
}
}
}

else if(cmd=="list_groups"){
if(uid=="" || loggedin[uid]==0)write(new_socket, "User is not logged in!!", 38);
string r="Groups are: ";
map<string,string>::iterator it=admin.begin();
for(auto it=admin.begin();it!=admin.end();it++){
r=r+it->first+" ";
}
line=new_socket+" client asked to list groups ";
fout<<line<<endl;
fout.flush();
write(new_socket, &r[0], 68);
}

else if(cmd=="show_current"){
write(new_socket, &uid[0], 58);
}

else{
line=new_socket+" client gave in wrong command ";
fout<<line<<endl;
write(new_socket, "Wrong command!!!!", 58);
}
printf("%s\n", buffer);

}
}
int main(int argc,char *argv[]){

fout.open("../logfile1.txt");
line="hello starting server";
fout<<line;
fout.flush();
//declarations of variables going to be used majorly
string ip;int port;
int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[1024] = { 0 };
char* hello = "Hello from server";

fstream fp;//declare file pointer
fp.open(argv[1],ios_base::in);//opened in reading mode the tracker_file.txt
if(!fp.is_open()){
cout<<"Tracker_info.txt does not exists";
exit(1);
}
string t;
int f=0;
while(getline(fp,t)){
f++;
if(f==1)ip=t;//ip address
if(f==2)port=stoi(t);//port
if(f==2)break;//we have read 2 lines containing ip and port no of tracker
}
//cout<<ip<<" "<<port;
// Creating socket file descriptor
if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
perror("socket failed");
exit(EXIT_FAILURE);
}
// Forcefully attaching socket to the port
if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
perror("setsockopt");
exit(EXIT_FAILURE);
}
address.sin_family = AF_INET;
if(inet_pton(AF_INET, &ip[0], &address.sin_addr)<=0)  {
printf("\nInvalid address/ Address not supported \n");
return -1;
}
address.sin_port = htons(port);
// Forcefully attaching socket to the port 8080
if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) {
perror("bind failed");
exit(EXIT_FAILURE);
}
if (listen(server_fd, 3) < 0) {
perror("listen");
exit(EXIT_FAILURE);
}

vector<thread> v;
pthread_t th;
if(pthread_create(&th, NULL, fun, NULL) == -1){
perror("pthread");
exit(EXIT_FAILURE);
}

while(true){
if ((new_socket= accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
perror("accept");
exit(EXIT_FAILURE);
}
line=new_socket+" client connected to tracker ";
fout<<line<<endl;
fout.flush();
v.push_back(thread(commands, new_socket));
}
for(auto i=v.begin(); i!=v.end(); i++){
if(i->joinable()) i->join();
}
return 0;
}
