#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>
using namespace std;
#define errorStart "\033[1;31m"
#define printEnd    "\033[0m"
pthread_t client_serve;
pthread_t client_t;
pthread_attr_t threadAttr_;
int serverFD;
//***********************************| DATASETS |*******************************************
struct fileData{
          string f_name;
          string f_path;
          string ip;
          string port;
          string user_id;
          string f_size;
          int noOfChunks;
          string fileChunks;
          string SHA_of_chunks;
};
struct group{      //all the info related to group..
    string admin;
    vector<string> members;
    vector<string> pendingReq;
    vector<fileData> files;

};

map<string,string> userPORT;
map<string,string> userIP;
map<string,string> LoginDetails; //to store username and password..
map<string,int> activeStatus; //to store active status of user..
map<string,string> FileSize;
map<string,group> groupMeta;   //to store groupId and there admin..
map<string,vector<string>> groupMembers; //stores group ID and all the members of the groups..
void* serveClient(void*);
long GetFileSize(std::string filename)
    {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
    }
//splitting the input command into parameters..
 vector<string> getCommands(string input,char del){
     vector<string> v;
     string temp="";
     for(int i=0;i<input.length();i++){
        if(input[i] == del){
            v.push_back(temp);
            temp="";
        }else temp=temp+input[i];
     }
            v.push_back(temp);
     return v;
 }

 struct thread_args {
       string t1_port;
       int serverFD;
 };
//****************************************************|  server to listen  |***************************************************
void* trackerServer(void* arg){
    //thread_args argv = *(thread_args *)arg;
   // string ServePort=argv->t1_port;
     string ServePort = *(string *)arg;
      struct sockaddr_in serverAddress; //structure specifies a transport address and port for the AF_INET address family
      serverFD=socket(AF_INET,SOCK_STREAM,0);
       //argv->serverFD = serverFD;
    if(serverFD<0) 
    {
        cout<<errorStart<<"error While Connecting Client"<<printEnd<<endl;
        exit(1);
    }
    //set memory with '0' at address of serveraddress..
    memset(&serverAddress, '0', sizeof(serverAddress));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port=htons(stoi(ServePort));   //htons:'host'to'network'short/long
    int opt=1;
  if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        cout<<"SETSOCKOPT"<<endl;
             exit(1);
    }
    if(bind (serverFD  , (struct sockaddr *)&serverAddress , sizeof ( serverAddress ) )<0)
    {
        cout<<errorStart<<"ERROR WHILE BINDING"<<printEnd<<endl;
        exit(1);
    }   

    if(listen (serverFD, 30)==0)
    {
        //cout<<"LISTENING --------"<<endl;
    }
    else
    {
        cout<<errorStart<<"ERROR WHILE LISTENING"<<printEnd<<endl;
        exit(1);
    }
    int addrlen = sizeof(sockaddr);
    int NewSocket;
    while(1)
    {
        if((NewSocket=accept( serverFD , (struct sockaddr *)&serverAddress , (socklen_t*)&addrlen))<0)
        {
        cout<<errorStart<<"ERROR IN CONNECTION STABLISHMENT"<<printEnd<<endl;
            exit(1);
        }
        int* newsock=(int *)malloc(sizeof(*newsock));
           *newsock=NewSocket;
           
        if(pthread_create(&client_t,0,serveClient,newsock)<0)
        {   
            cout<<errorStart<<"could not create client thread"<<printEnd<<endl;
            exit(1)   ;
        }
        
    }  
    return arg; 
}

//****************************************************|  Main  |***************************************************
int main(int argc, char* argv[]){
     printf("\033[H\033[J");

     string tInfoFile=string(argv[1]);
     vector<string> getIP_PORT;
     fstream f_stream(tInfoFile,ios::in);      
      string ipPort;
      while(getline(f_stream,ipPort,'\n'))
      {
        getIP_PORT.push_back(ipPort);
      }
      vector<string> IPort;
       IPort=getCommands(getIP_PORT[0],':');
      string t1_ip=IPort[0];
      string t1_port=IPort[1];
       IPort=getCommands(getIP_PORT[1],':');
      string t2_ip=IPort[0];
      string t2_port=IPort[1];

     string tr_NO=string(argv[2]);
     cout<<endl<<"                           "<<"\033[1;30;45m"<<" || <Tracker> IP : "<<t1_ip<<" PORT : "<<t1_port<<" || "<<printEnd<<endl;
    //serverFD: socket file descriptor
     int response= pthread_attr_init(&threadAttr_);
    if (response != 0) {
        cout<<errorStart<<"Attribute creation failed"<<printEnd<<endl;
        exit(EXIT_FAILURE);
    }
    if(pthread_create( &client_serve , &threadAttr_ , trackerServer , (void*)&t1_port) < 0)
    {
        cout<<errorStart<<"thread creation failed"<<printEnd<<endl;
        return 1;
    }
    while(true){         
    string inp;
    cin>>inp;
    if(inp=="quit") break; 
    else cout<<errorStart<<"invailid Input!"<<printEnd<<endl;
    }
    //pthread_exit (NULL);
    //shutdown(serverFD, SHUT_RDWR);
    return 0;
}

//****************************************************|  thread to serve clients  |***************************************************
void* serveClient(void* New_Socket){
      int NewSocket = *(int *)New_Socket;
                    cout<<"         =================================\n";
   cout<<"             "<<"\033[5;30;47m"<<"  New client connected: "<<NewSocket<<"  "<<printEnd<<endl;
                    cout<<"         =================================\n";

      int Session=0;
      string userID="";
   while(1){
    char buffer[1024] = { 0 };
    string retVal = "hii client from tracker";
    int valread = read(NewSocket, buffer, 1024);
         if(valread==0){
                              cout<<"         =================================\n";
   cout<<"             "<<"\033[1;37;41m"<<"  client disconnected: "<<NewSocket<<"  "<<printEnd<<endl;
                    cout<<"         =================================\n";

          break;
          }
           string buf=string(buffer);
    vector<string> command=getCommands(buf,' ');
    if(command[0]== "create_user" || command[0]== "login" ||command[0]== "quit" || Session == 1){
        if(command[0] == "create_user")
        {
            if(LoginDetails.find(command[1])!=LoginDetails.end())
               {
                  string retVal = "invailid";
                 send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
               }
           else{
                 LoginDetails.insert({ command[1], command[2] });
                 activeStatus.insert({ command[1], 0 });
                 string retVal = "vailid";
                 send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
               }
        }
        else if(command[0] == "login")
        {
            if(LoginDetails.find(command[1])!=LoginDetails.end())
            {
                if(LoginDetails[command[1]] == command[2]){

                    if(activeStatus[command[1]]==1){
                         string retVal = "already";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                    }
                    else{
                 Session=1;
                 userID=command[1];
                 userIP[userID]=command[3];
                 userPORT[userID]=command[4];
                 activeStatus[userID]=1;
                string retVal = "vailid";
                 send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                    }
             }
             else
            {
                 string retVal = "invailid";
                send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
         }
            else
            {
                string retVal = "invailid";
                send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
         }
         else if(command[0] == "create_group"){
            if(groupMeta.find(command[1])!=groupMeta.end())
            {
              string retVal = "invailid";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
            else
            {
                 group g;
                 g.admin=userID;
                 g.members.push_back(userID);
             groupMeta.insert({ command[1], g });
                string retVal = "vailid";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
         }
         else if(command[0] == "join_group"){
          if(groupMeta.find(command[1])==groupMeta.end())
            {
              string retVal = "invailid";
            send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
            else if(groupMeta[command[1]].admin == userID){
                groupMeta[command[1]].members.push_back(userID);
                string retVal = "uAdmin";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
               }
            else if (find(groupMeta[command[1]].members.begin(), groupMeta[command[1]].members.end(), userID) != groupMeta[command[1]].members.end() )
                   {
                     string retVal = "alreadyValid";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                   }  
              
            else{
                groupMeta[command[1]].pendingReq.push_back(userID);
                   string retVal = "vailid";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
         }
         else if(command[0] == "leave_group"){
           if(groupMeta.find(command[1])==groupMeta.end())
            {
               string retVal = "invailid";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
            else if (find(groupMeta[command[1]].members.begin(), groupMeta[command[1]].members.end(), userID) == groupMeta[command[1]].members.end() )
                   {
                     string retVal = "notPresent";
                        send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                   }  
            else if(groupMeta[command[1]].admin == userID){
                groupMeta[command[1]].members.push_back(userID);
                string retVal = "uAdmin";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
            else{
                 groupMeta[command[1]].members.erase(find(groupMeta[command[1]].members.begin(),groupMeta[command[1]].members.end(),userID));
                string retVal = "vailid";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
         }
         else if(command[0] == "list_requests"){
            if(groupMeta[command[1]].admin == userID){
                  string s="";
                  for (int i = 0; i < groupMeta[command[1]].pendingReq.size(); i++) 
                        s=s+","+groupMeta[command[1]].pendingReq[i];
                    if(s.length()==0) 
                 {
                      string retVal = "nothing";
                     send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
                 }
                 else
                 {
                       char* retVal=new char[s.length()+1];
                        strcpy(retVal,s.c_str());
                        send(NewSocket, retVal, strlen(retVal), 0);
                 }

              }
              else
              {
                  string retVal = "invailid";
                send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
              }
         }
         else if(command[0] == "accept_request"){
             if(groupMeta.find(command[1])==groupMeta.end())
            {
            string retVal = "invailid";
            send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
            else if(groupMeta[command[1]].admin != userID){ 
                string retVal = "notAdmin";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
             else if (find(groupMeta[command[1]].pendingReq.begin(), groupMeta[command[1]].pendingReq.end(), command[2]) != groupMeta[command[1]].pendingReq.end() )
                   {
                      groupMeta[command[1]].members.push_back(command[2]);
                     groupMeta[command[1]].pendingReq.erase(find(groupMeta[command[1]].pendingReq.begin(),groupMeta[command[1]].pendingReq.end(),command[2]));  
                      string retVal = "vailid";
                      send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                   } 

            else{
                  string retVal = "notPresent";
                send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                }
         }
         else if(command[0] == "list_groups"){
                  string s="";
                for(map<string,group>::const_iterator it = groupMeta.begin(); it != groupMeta.end(); it++)
                {
                      s=s+","+it->first;
                 }
                 if(s.length()==0) 
                 {
                      string retVal = "invailid";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                 }
                 else
                 {
                       char* retVal=new char[s.length()+1];
                     strcpy(retVal,s.c_str());
                     send(NewSocket, retVal, strlen(retVal), 0);
                 }
           }
         else if(command[0] == "list_files"){
            if(groupMeta.find(command[1])==groupMeta.end())
            {
              string retVal = "invailid";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
            else{
                   string s="";
                  for (int i = 0; i < groupMeta[command[1]].files.size(); i++) 
                        s=s+","+groupMeta[command[1]].files[i].f_name;
     if (find(groupMeta[command[1]].members.begin(), groupMeta[command[1]].members.end(), userID) == groupMeta[command[1]].members.end() )
     {
        string retVal = "notMember";
                     send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
     }
                   else if(s.length()==0) 
                 {
                      string retVal = "nothing";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                 }
                 else
                 {
                       char* retVal=new char[s.length()+1];
                        strcpy(retVal,s.c_str());
                        send(NewSocket, retVal, strlen(retVal), 0);
                 }

                
            }
         }
         else if(command[0] == "upload_file"){
     if(groupMeta.find(command[1])==groupMeta.end())
            {
            string retVal = "invailid";
            send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
     else if (find(groupMeta[command[1]].members.begin(), groupMeta[command[1]].members.end(), userID) != groupMeta[command[1]].members.end() )
             {  
                fileData f;
                f.f_name=command[2];
                f.f_size=command[3];
                if(FileSize.find(command[2])==FileSize.end())
                FileSize[command[2]]=command[3];
                f.f_path=command[4];
                f.ip=command[5];
                f.port=command[6];
                f.noOfChunks=stoi(command[7]);
                int shaSize=stoi(command[8]);
                f.fileChunks=command[9];
                f.user_id=userID;
                groupMeta[command[1]].files.push_back(f);
               string retVal = "vailid";
               send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
               int buf_size=shaSize;
               char buffer_[buf_size] = { 0 };
                read(NewSocket, buffer_, buf_size);
                //cout<<strlen(buffer_)<<endl;
                f.SHA_of_chunks=string(buffer_).substr(0, buf_size);
               send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
              }
          else{
              string retVal = "notPresent";
               send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
          }
      }
         else if(command[0] == "download_file"){
             if(groupMeta.find(command[1])==groupMeta.end())
            {
            string retVal = "invailid";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
            }
            else{
                  vector<int> index;
                  int flag=0;
                  for (int i = 0; i < groupMeta[command[1]].files.size(); i++) {
                     if(groupMeta[command[1]].files[i].f_name == command[2])
                     {
                         flag=1;
                         index.push_back(i);
                     }
                  }
                    if(flag == 0) 
                 {
                      string retVal = "nothing";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                 }
                 else
                 {   

                      //int s=index.size();
                     // string ss=to_string(s);
                      //send(NewSocket, ss.c_str(), ss.length(), 0);

                    string s=FileSize[command[2]]+";";
                     for(int i=0;i<index.size();i++){
                        if(activeStatus[groupMeta[command[1]].files[index[i]].user_id] == 0)
                            continue;
                      string u_id=groupMeta[command[1]].files[index[i]].user_id ;
                      string fsize=groupMeta[command[1]].files[index[i]].f_size ;
                      string fpath=groupMeta[command[1]].files[index[i]].f_path ;
                      string ip_=userIP[u_id] ;
                      string port_=userPORT[u_id];
                      string noOfChunks_=to_string(groupMeta[command[1]].files[index[i]].noOfChunks);
                      string fileChunks_=groupMeta[command[1]].files[index[i]].fileChunks;
                        s=s+fsize+","+fpath+","+ip_+","+port_+","+noOfChunks_+","+fileChunks_+";";
                      }
                      char* retVal=new char[s.length()+1];
                        strcpy(retVal,s.c_str());
                      send(NewSocket, retVal, strlen(retVal), 0);
                      
                 }

                
            }
         }
         else if(command[0] == "stop_share"){
           if(groupMeta.find(command[1])==groupMeta.end())
            {
            string retVal = "invailid";
            send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
            }
            else{
                  int index;
                  int flag=0;
                  for (int i = 0; i < groupMeta[command[1]].files.size(); i++) {
                     if(groupMeta[command[1]].files[i].f_name == command[2] && groupMeta[command[1]].files[i].user_id==userID)
                     {
                         flag=1;
                         index=i;
                     }
                  }
                    if(flag == 0) 
                 {
                      string retVal = "nothing";
                     send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                 }
                 else
                 {   
                      groupMeta[command[1]].files.erase( groupMeta[command[1]].files.begin() + index );   
                      string retVal = "vailid";
                      send(NewSocket,  retVal.c_str(), retVal.length()+1, 0);
                 }

                
            }
         }
         else if(command[0] == "logout"){
            Session=0;
           activeStatus[userID]=0;
            string retVal = "logged out successfully";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
         }
         else if(command[0] == "quit"){                  
                  cout<<"         =================================\n";
   cout<<"             "<<"\033[1;37;41m"<<"  client disconnected: "<<NewSocket<<"  "<<printEnd<<endl;
                    cout<<"         =================================\n";

           break;
         } 
   
     }
     else{
            string retVal = "Please login first..";
            send(NewSocket, retVal.c_str(), retVal.length()+1, 0);
     }

    }

        string endstr= "Successfully disconnected from TRACKER"; 
    send(NewSocket, endstr.c_str(), endstr.length()+1, 0);
    return New_Socket;
}
//*********************************************| Testing |******************************************************************