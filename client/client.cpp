#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
#define errorStart "\033[1;31m"
#define cmdStart "\033[5;34m"
#define oprStart "\033[0;34m"
#define TrOpStart "\033[0;32m"
#define printEnd    "\033[0m"
#define CHUNK_SIZE 524288
#define CHUNKSIZE 524288
pthread_t threadServe_;
pthread_t client_t;
pthread_attr_t threadAttr_;
vector<string> D_files,C_files,D_gid,C_gid;  // stores the DATA of downloading or completed files
int seederStatus[100]={0};
struct SHA_DATA
{
    string fileName;
    string fileSize;
    map<int, string> shaOfPieces;
};

vector<struct SHA_DATA> chunkSHA_perFile;
unordered_map<string,string> hASH_OfFiles;
struct chunkData{
    vector<int> chunkExists;
};
map<string,chunkData > chunksNo;
string convertToString(char* a)
{
    string s = a;
    return s;
}
long long GetFileSize(string filename)
 {
    // struct stat stat_buf;
    // int rc = stat(filename.c_str(), &stat_buf);
    // return rc == 0 ? stat_buf.st_size : -1;

    FILE *F_pntr = fopen(filename.c_str(), "r");
            if (F_pntr == NULL)
            {
                printf("File Not Found!\n");
            }
            fseek(F_pntr, 0L, SEEK_END);
            long long fileSize = ftell(F_pntr);
            fclose(F_pntr);
     return fileSize;
 }
string getFileName(string input){
    char del='/';
     string temp="";
     for(int i=0;i<input.length();i++){
        if(input[i] == del){
            temp="";
        }else temp=temp+input[i];
     }
    return temp;
}
//**************************************************|  Receive File  |*************************************************
void receiveFile(int new_socket,long long filesize,string fname ){
   cout<<endl<<oprStart<<"      Receiveing File........"<<printEnd<<endl;
    FILE *F_pntr = fopen ( fname.c_str() , "wb" );
    if (F_pntr == NULL)
            {
               cout<<errorStart<<"  file not found!"<<printEnd<<endl;
                filesize=-100;
            }
    size_t chunk=524288;
    char Buffer [chunk] ;
    long long n;
    memset(Buffer , '\0', chunk);
    while (( n = recv( new_socket , Buffer ,   chunk, 0) ) > 0  &&  filesize > 0)
    {
        fwrite (Buffer , sizeof (char), n, F_pntr);
        memset ( Buffer , '\0', chunk);
        filesize = filesize - n;
        if(n<=0 || filesize<=0)
        {
            break;
        }
    }
    if(filesize == -100)  cout<<errorStart<<"      File Download failed..  "<<printEnd<<endl;
        else  cout<<oprStart<<"      File Downloaded  "<<printEnd<<endl;
    fclose (F_pntr);
}
void transferFile(int socketID, long long filesize, string fname){
   cout<<oprStart<<"          Sending File..............."<<printEnd<<endl;
        size_t chunk=524288;
        FILE *F_pntr= fopen(fname.c_str(),"rb");
        if (F_pntr == NULL)
            {
                cout<<errorStart<<"  file not found!"<<printEnd<<endl;
                char t[0];
                send (socketID,t, 0, 0 );
                return;
            }
        char Buffer[chunk];
        long long n;
        while ( ( n = fread( Buffer , sizeof(char) , chunk , F_pntr ) ) > 0  && filesize > 0 )
        {
            send (socketID,Buffer, n, 0 );
            memset ( Buffer , '\0', chunk);
            filesize = filesize - n ;
            if(n<=0 || filesize <=0)
            {
                break;
            }
        }
        cout<<oprStart<<"          File sent   "<<printEnd<<endl;
    fclose (F_pntr);
}
//Send and recieve data from tracker..
string getInfoFromTracker(string input,int socketID){
           char* msg_char=new char[input.length()+1];
           strcpy(msg_char,input.c_str());
           send(socketID,msg_char,strlen(msg_char),0);
           char buffer[4096] = { 0 };
           read(socketID, buffer, 4096);
           return string(buffer);
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
 //get sha of that file..
 vector<string> cal_SHA(string f_path){
    vector<string> sha;
    FILE *filePoiner = fopen(f_path.c_str(), "r+");
    if (filePoiner == NULL)
    {
        cout<<errorStart<<"file can't be opened"<<printEnd<<endl;
        return sha;
    }
    unsigned char buffer[CHUNK_SIZE];
    unsigned char hASH[20];
    char p[40];
    string chunkString = "";
    int n;
    int countChunks = 0;
    while ((n = fread(buffer, 1, sizeof(buffer), filePoiner)) > 0)
    { 
        SHA1(buffer, n, hASH);
        bzero(buffer, CHUNK_SIZE);
        for (int i = 0; i < 20; i++)
            sprintf(p + 2 * i, "%02x", hASH[i]); 
        sha.push_back(p);
        countChunks++;
    }
    return sha;
 }

void transferChunks(int p_NewSOCKID,int ChunkNo,string filename ,int FD){
                // Send Data from Peer
               int  nBytes = 0;
               int TotalBytes = 0;
               int BlkSize =32*1024;
              char Buffer[BlkSize];
                int ChunkOffset = ((ChunkNo - 1) * CHUNKSIZE);
                int i=1;
                while (true)
                {
                    memset(Buffer , '\0', BlkSize);
                    ChunkOffset += nBytes;              
                    nBytes = pread(FD, Buffer, BlkSize, ChunkOffset);
                    TotalBytes += nBytes;
                     if (nBytes <=0)
                    {
                        break;
                    }
                    if ( TotalBytes == CHUNKSIZE)
                    {
                          nBytes = send(p_NewSOCKID, Buffer, nBytes, 0);
                        break;
                    }
                    else
                    {
                        nBytes = send(p_NewSOCKID, Buffer, nBytes, 0);
                    }
                    usleep(100000);
                }
  
}

void peer2peer(string fsize,string file_path, string ip,string port,string DestOfFile, string chunks ,int seedNo,int FD){
    int socket_ = 0,read_;
    struct sockaddr_in servAddr;
    char buffer[1024] = {0};
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout<<errorStart<<"  Socket Creation failed!"<<printEnd<<endl;
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(stoi(port));
     char* cip_=new char[ip.length()+1];
      strcpy(cip_,ip.c_str());
    if(inet_pton(AF_INET, cip_, &servAddr.sin_addr)<=0)
    {
        cout<<errorStart<<"  Invailid peer address!  "<<printEnd<<endl;
    }
    if (connect(socket_, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        cout<<errorStart<<" connection failed to peer!  "<<printEnd<<endl;
    }
      string s=fsize+","+file_path+","+chunks+","+to_string(seedNo);
      char* retVal=new char[s.length()+1];
      strcpy(retVal,s.c_str());
      send(socket_, retVal, strlen(retVal), 0);
      long long filesize=stoll(fsize);
                       long long fileSize=stoull(fsize);
                       long long C_Size = 524288;
                       int LastCnk= (int)ceil(fileSize / (C_Size * 1.0));
                       int LastcnkBytes=(int)(fileSize-C_Size*(LastCnk-1));
       vector<string> chunks_list=getCommands(chunks,':');
         //FILE *FD = fopen ( DestOfFile.c_str() , "wb" );
         if (FD < 0 )
            {
                cout<<errorStart<<"  file not found!"<<printEnd<<endl;

      seederStatus[seedNo]=-1;
                return ;
            }
           for(int i=0;i<chunks_list.size()-1;i++){
        int PeerSockID=socket_ ;int p_ChunkNo=stoi(chunks_list[i]) ;string filename=DestOfFile;
      // cout<<"  recieve chunks called : "<<p_ChunkNo;
                int ChunkOffset = ((p_ChunkNo - 1) * CHUNKSIZE);
                int TotalBytes = 0;
              int  nBytes = 0;
              int BlkSize =32*1024;
              char Buffer[BlkSize];
              int k=1;
                while (true)
                {
                    memset(Buffer , '\0', BlkSize);
                    ChunkOffset += nBytes;
                     if(p_ChunkNo==LastCnk && TotalBytes>=LastcnkBytes)break;
                    nBytes = recv(PeerSockID, Buffer, BlkSize,0);
                    if (nBytes <= 0 )
                    {
                        break;
                    }
                    else
                    {
                               
                        pwrite(FD, Buffer, nBytes, ChunkOffset);
                        TotalBytes += nBytes;
                        if(TotalBytes>=CHUNKSIZE) break;
                    }
                }
  }
      
      send(socket_, retVal, strlen(retVal), 0);
      seederStatus[seedNo]=1;
     // close(socket_);
      return ;
}
//thread to work as a server for the client..
void *start_Server(void* c_PORT){
     struct sockaddr_in serverAddress; //structure specifies a transport address and port for the AF_INET address family
    //serverFD: socket file descriptor
    string ServePort = *(string *)c_PORT;
    int  sock_FD=socket(AF_INET,SOCK_STREAM,0);
    if(sock_FD<0) 
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
  if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    {
        cout<<"SETSOCKOPT"<<endl;
             exit(1);
    }
    if(bind (sock_FD  , (struct sockaddr *)&serverAddress , sizeof ( serverAddress ) )<0)
    {
        cout<<errorStart<<"ERROR WHILE BINDING"<<printEnd<<endl;
        exit(1);
    }   

    if(listen (sock_FD, 30)==0)
    {
        //cout<<"LISTENING --------"<<endl;
    }
    else
    {
        cout<<errorStart<<"ERROR WHILE LISTENING"<<printEnd<<endl;
        exit(1);
    }
    int addrlen = sizeof(sockaddr);
    while(true){
    int Newsocket;
    if((Newsocket=accept( sock_FD , (struct sockaddr *)&serverAddress , (socklen_t*)&addrlen))<0)
        {
            cout<<errorStart<<"ERROR IN CONNECTION STABLISHMENT"<<printEnd<<endl;
            exit(1);
        }
        cout<<endl<<oprStart<<"          connected from PORT:   "<<serverAddress.sin_port<<endl;
                        cout<<"          socket created : "<<Newsocket<<endl;   
                  
           char buffer[1024] = { 0 };
           int valread = read(Newsocket, buffer, 1024);
         string info=string(buffer);
         //cout<<info<<endl;
               vector<string> list=getCommands(info,',');
                        long long filesize=stoll(list[0]);
                           string fname=list[1];
                           string chunks=list[2];
                            int seedNo=stoi(list[3]);
                           vector<string> chunks_list=getCommands(chunks,':');
                           cout<<"          Chunks to send : "<<printEnd;
                           for(int i=0;i<chunks_list.size()-1;i++)
                            cout<<chunks_list[i]<<" ";

         cout<<endl<<oprStart<<"          conn. requested for file : "<<fname<<printEnd<<endl;
        //transferFile(Newsocket, filesize, fname);

        cout<<oprStart<<"          Sending Chunks..............."<<printEnd<<endl;

        //FILE *F_pntr= fopen(fname.c_str(),"rb");

       int F_pntr = open(fname.c_str(), O_RDONLY);
         if (F_pntr < 0 )
            {
                cout<<errorStart<<"  file not found!"<<printEnd<<endl;
                char t[0];
                send (Newsocket,t, 0, 0 );
             seederStatus[seedNo]=-1;
                return c_PORT;
            }
          //cout<<endl<<" file to transfer : "<<fname<<endl;
          for(int i=0;i<chunks_list.size()-1;i++){
            //transferChunks(Newsocket,stoi(chunks_list[i]),fname,F_pntr);

            int p_NewSOCKID=Newsocket;int ChunkNo=stoi(chunks_list[i]) ;string filename=fname; int FD=F_pntr;
                // Send Data from Peer
               int  nBytes = 0;
               int TotalBytes = 0;
               int BlkSize =32*1024;
              char Buffer[BlkSize];
                int ChunkOffset = ((ChunkNo - 1) * CHUNKSIZE);
                //int i=1;
                while (true)
                {
                    memset(Buffer , '\0', BlkSize);
                    ChunkOffset += nBytes;              
                    nBytes = pread(FD, Buffer, BlkSize, ChunkOffset);
                    TotalBytes += nBytes;
                     if (nBytes <=0)
                    {
                        break;
                    }
                    if ( TotalBytes == CHUNKSIZE)
                    {
                          nBytes = send(p_NewSOCKID, Buffer, nBytes, 0);
                        break;
                    }
                    else
                    {
                        nBytes = send(p_NewSOCKID, Buffer, nBytes, 0);
                    }
                    usleep(1000);
                }
  


        }
        cout<<oprStart<<"          File sent   "<<printEnd<<endl;
           valread = read(Newsocket, buffer, 1024);
         cout<<oprStart<<"          connection closed to socket :"<<Newsocket<<printEnd<<endl;
}
return c_PORT;
}

void upload_filefc(string command,string file_path,string group_id ,string c_IP,string c_PORT,int socketID){
string f_name=getFileName(file_path);
              vector<string> file_SHA_ofChunks=cal_SHA(file_path);
              long long f_size=GetFileSize(file_path);   
              long long C_Size = 524288;
              long long  ChunkCount= ceil(f_size / (C_Size * 1.0));
              struct chunkData cc;
              string ChunkInfo="";
              for(long long i=1;i<ChunkCount;i++){
              chunksNo[f_name].chunkExists.push_back(i);
              ChunkInfo+=to_string(i)+":";
              }
              ChunkInfo+=to_string(ChunkCount);
              struct SHA_DATA obj;
              obj.fileName = f_name;
              obj.fileSize=f_size;
              string file_SHA;
               for (int i = 0; i < file_SHA_ofChunks.size(); i++)
                {
                obj.shaOfPieces[i] = file_SHA_ofChunks[i];
                file_SHA = file_SHA + obj.shaOfPieces[i];
                }
               hASH_OfFiles[f_name] = file_SHA;
               chunkSHA_perFile.push_back(obj);
              string Tokens="";
               for(int i=0;i<file_SHA_ofChunks.size();i++){
                   Tokens+=file_SHA_ofChunks[i]+" ";
                        }
                        string shaSize=to_string(Tokens.length());
               string input=command+" "+group_id+" "+f_name+" "+to_string(f_size)+" "+file_path+" "+c_IP+" "+c_PORT+" "+to_string(ChunkCount)+" "+shaSize+" "+ChunkInfo;               
               string op=getInfoFromTracker(input,socketID);
                if(op == "vailid"){
                    //cout<<endl<<input<<endl<<" Tokens: "<<Tokens<<endl<<Tokens.length()<<endl;
                      //op=getInfoFromTracker(Tokens,socketID);
                       char* msg_char=new char[Tokens.length()+1];
                        strcpy(msg_char,Tokens.c_str());
                        send(socketID,msg_char,strlen(msg_char),0);
                        char buffer[4096] = { 0 };
                        read(socketID, buffer, 4096);
                       op=string(buffer);
                      if(op == "vailid")
                    cout<<TrOpStart<<" file shared"<<printEnd<<endl;
                      else 
                cout<<errorStart<<"  something wrong happened : "<<group_id<<" "<<printEnd<<endl;
                      
                }
              else if(op == "invailid")
                cout<<errorStart<<"  Group doesn't exists : "<<group_id<<" "<<printEnd<<endl;
              else 
                cout<<errorStart<<"  You are not a member of group : "<<group_id<<" "<<printEnd<<endl;
}
int main(int argc, char* argv[])
{
      if(argc!=3)
  {
    cout<<errorStart<<" Command line argument should be in formate:: <IP>:<PORT> traker_info.txt"<<printEnd<<endl;
    perror("Error in command line argument list!");
    return -1;
  }
      vector<string> ip_port=getCommands(string(argv[1]),':');
     string c_IP=ip_port[0];
     string c_PORT=ip_port[1];
     
     string tInfoFile=string(argv[2]);
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

      char* c_t1_ip=new char[t1_ip.length()+1];
      strcpy(c_t1_ip,t1_ip.c_str());
       printf("\033[H\033[J");
      cout<<endl<<"                           "<<"\033[1;30;43m"<<" | <client> IP : "<<c_IP<<" PORT : "<<c_PORT<<" | "<<printEnd<<endl;

    struct sockaddr_in serverAddress; //structure specifies a transport address and port for the AF_INET address family
    //socketID: socket file descriptor

    int response= pthread_attr_init(&threadAttr_);
    if (response != 0) {
        cout<<errorStart<<"Attribute creation failed"<<printEnd<<endl;
        exit(EXIT_FAILURE);
    }
    if(pthread_create( &threadServe_ , &threadAttr_ , start_Server , (void*)&c_PORT) < 0)
    {
        cout<<errorStart<<"thread creation failed"<<printEnd<<endl;
        return 1;
    }
    int socketID=socket(AF_INET,SOCK_STREAM,0);
    if(socketID<0) 
    {
        cout<<errorStart<<"ERROR WHILE CREATING SOCKET"<<printEnd<<endl;
        exit(1);
    }
    //set memory with '0' at address of serveraddress..
    memset(&serverAddress, '0', sizeof(serverAddress));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(stoi(t1_port));   //htons:'host'to'network'short/long
     //converts the character string src into a network address structure(binary) in the af address family
    if (inet_pton(AF_INET, c_t1_ip, &serverAddress.sin_addr) <= 0)
    {
        cout<<errorStart<<"INVAILID ADDRESS"<<printEnd<<endl;
        exit(1);
    }
    
    int new_socket;
         if(new_socket=connect(socketID,(struct sockaddr *)&serverAddress  , sizeof(serverAddress))<0)
         {
        cout<<errorStart<<"CONNECTION FAILED"<<printEnd<<endl;
        exit(1);
         }

      int Session=0;
    while(true){
          cout<<cmdStart<<"$:- "<<printEnd;
         string command;
         cin>>command;

    if(command== "create_user" || command== "login" ||command== "quit" || Session == 1){
         if(command == "quit") {
            string input="quit";
           cout<<getInfoFromTracker(input,socketID)<<endl;
            break;
        }
         //On command create_user..

         else if(command == "create_user")
         {
            string user_id,password;
            cin>>user_id>>password;
            string input=command+" "+user_id+" "+password;
             string op=getInfoFromTracker(input,socketID);
             if(op == "vailid")
             cout<<TrOpStart<<"  User Created"<<printEnd<<endl;
             else  cout<<errorStart<<"  User already exists.."<<printEnd<<endl;
         }
         else if(command == "login")
         {
             string user_id,password;
            cin>>user_id>>password;
             string input=command+" "+user_id+" "+password+" "+c_IP+" "+c_PORT;   
             string op=getInfoFromTracker(input,socketID);
             if(op == "vailid"){
                 Session = 1;
             cout<<TrOpStart<<"  Successfully LoggedIn"<<printEnd<<endl;
         }
          else if(op == "already")  cout<<errorStart<<"  You are already LoggedIn somewhere.."<<printEnd<<endl;
             else  cout<<errorStart<<"  Invailid credentials"<<printEnd<<endl;
         }
         else if(command == "create_group"){
              string group_id;
              cin>>group_id;
             string input=command+" "+group_id; 
             string op=getInfoFromTracker(input,socketID);
             if(op == "vailid")
                    cout<<TrOpStart<<"  Group created.."<<printEnd<<endl;
             else  cout<<errorStart<<"  Group already exists.."<<printEnd<<endl;
         }
           else if(command == "join_group"){
            string group_id;
              cin>>group_id;
               string input=command+" "+group_id;         
           string op=getInfoFromTracker(input,socketID);
              if(op == "vailid")
                    cout<<TrOpStart<<"  Request pending to join Group :"<<group_id<<" "<<printEnd<<endl;
              else if(op == "alreadyValid")
                    cout<<TrOpStart<<"  already joined Group :"<<group_id<<" "<<printEnd<<endl;
              else if(op == "uAdmin") 
                cout<<errorStart<<"  You are already admin of the group. "<<printEnd<<endl;
              else  cout<<errorStart<<"  Group doesn't exists.."<<printEnd<<endl;
         }
         else if(command == "leave_group"){
              string group_id;
              cin>>group_id;
              string input=command+" "+group_id;
              string op=getInfoFromTracker(input,socketID);
              if(op == "vailid")
                    cout<<TrOpStart<<"  leave group :"<<group_id<<"  Done. "<<printEnd<<endl;
              else if(op == "notPresent")
                    cout<<errorStart<<"  You haven't joined group :"<<group_id<<" "<<printEnd<<endl;
              else if(op == "uAdmin") 
                cout<<errorStart<<"  You are already admin of the group, you can't leave. "<<printEnd<<endl;
              else  cout<<errorStart<<"  Group doesn't exists.."<<printEnd<<endl;
         }
         else if(command == "list_requests"){
            string group_id;
              cin>>group_id;
               string input=command+" "+group_id;
            string op=getInfoFromTracker(input,socketID);
             if(op == "invailid")
                 cout<<errorStart<<"  You aren't admin of the group.."<<printEnd<<endl;
             else if (op == "nothing")
                    {
                        cout<<errorStart<<" No pending requests!"<<printEnd<<endl;
                        continue;
                    }
                    else{
                        vector<string> list=getCommands(op,',');
                    cout<<">>>>"<<endl;
                     cout<<"       -----------------------------------------------------------  "<<endl;
                   for (int i = 1; i < list.size(); i++) 
                     cout<<"               "<<i<<". "<<TrOpStart<<list[i]<<printEnd<<" "<<endl;
                     cout<<"       -----------------------------------------------------------  "<<endl;

             }

         }
         else if(command == "accept_request"){
           string group_id,user_id;
              cin>>group_id>>user_id;
               string input=command+" "+group_id+" "+user_id;
             string op=getInfoFromTracker(input,socketID);
              if(op == "vailid")
                cout<<TrOpStart<<" Request accepted for group_id :"<<group_id<<" and user_id "<<user_id<<" "<<printEnd<<endl;
              else if(op == "invailid")
                cout<<errorStart<<"  Group doesn't exists : "<<group_id<<" "<<printEnd<<endl;
              else if(op == "notAdmin")
                cout<<errorStart<<"  You are not a admin of group : "<<group_id<<" "<<printEnd<<endl;
              else 
                cout<<errorStart<<"User id not present in pending list of group_id : "<<group_id<<" "<<printEnd<<endl;
         }
         else if(command == "list_groups"){
               string input=command;
              string op=getInfoFromTracker(input,socketID);
             if(op == "invailid")
                 cout<<errorStart<<"  No groups are present at this moment. "<<printEnd<<endl;
             else {  
               vector<string> list=getCommands(op,',');
               cout<<cmdStart<<">>>>"<<printEnd<<endl;

                     cout<<"       -----------------------------------------------------------"<<endl;
               for (int i = 1; i < list.size(); i++) 
                     cout<<"               "<<i<<". "<<TrOpStart<<list[i]<<printEnd<<" "<<endl;
                     cout<<"       -----------------------------------------------------------"<<endl;
             }
         }
         else if(command == "list_files"){
              string group_id;
              cin>>group_id;
               string input=command+" "+group_id;
            string op=getInfoFromTracker(input,socketID);
             if(op == "invailid")
                 cout<<errorStart<<"  Group doesn't exists.."<<printEnd<<endl;
             else if (op == "nothing")
                    {
                        cout<<errorStart<<"  No files available..."<<printEnd<<endl;
                        continue;
                    }
                else if (op == "notMember")
                    {
                        cout<<errorStart<<" You are not a member of this group ..."<<printEnd<<endl;
                        continue;
                    }
                    else{
                        vector<string> a=getCommands(op,',');
                          set<string> list;
                       for (int i = 1; i < a.size(); i++)
                          list.insert(a[i]);
               cout<<cmdStart<<">>>>"<<printEnd<<endl;
                     int kk=1;
                     cout<<"       -----------------------------------------------------------"<<endl;
                       for(auto& Files_: list)
                        cout<<"               "<<kk++<<". "<<TrOpStart<<Files_<<printEnd<<" "<<endl;
                     cout<<"       -----------------------------------------------------------"<<endl;
             }
         }
         else if(command == "upload_file"){
             string group_id,file_path;
              cin>>file_path>>group_id;
              upload_filefc(command,file_path,group_id,c_IP,c_PORT,socketID);
         }
         else if(command == "download_file"){
            string group_id,file_name,destination_path;
            cin>>group_id>>file_name>>destination_path;
            string input=command+" "+group_id+" "+file_name+" "+destination_path;
          string op=getInfoFromTracker(input,socketID);
              if(op == "invailid")
                cout<<errorStart<<"  Group doesn't exists : "<<group_id<<" "<<printEnd<<endl;
              else if(op == "nothing")
                cout<<errorStart<<"  file not shared before.. "<<printEnd<<endl;
            else{
                      D_files.push_back(file_name);
                      D_gid.push_back(group_id);
                      vector<string> seeders=getCommands(op,';');
                      long long fileSize=stoull(seeders[0]);
                       int NoOfSeedrs=seeders.size()-2;
                       long long C_Size = 524288;
                       int NoOfChunks= (int)ceil(fileSize / (C_Size * 1.0));
                       //getting chunk info of all seeders.....
                       int avail_C_atSeeder[NoOfSeedrs+1][NoOfChunks+1];
                       for(int k=0;k<=NoOfSeedrs;k++){
                       for(int i=0;i<=NoOfChunks;i++){
                        avail_C_atSeeder[k][i]=0;
                        }}
                           for(int i=1;i<=NoOfSeedrs;i++){
                              vector<string> temp=getCommands(seeders[i],',');
                                vector<string> chnks=getCommands(temp[5],':');
                                   for(int j=0;j<chnks.size();j++){
                                       avail_C_atSeeder[i][stoi(chnks[j])]=1;
                                   }
                                   
                           }
                     
                    int getChunksfor[NoOfChunks+1];
                       getChunksfor[0]=0;
                       int s=1;
                         for(int i=1;i<=NoOfChunks;i++)
                         {
                            for(int j=1;j<=NoOfSeedrs;j++){
                                if(avail_C_atSeeder[s][i]==1){
                                    getChunksfor[i]=s++;
                                    break;
                                }
                                else{
                                      s++;
                                      if(s>NoOfSeedrs) s=1;
                                }
                            }
                            if(s>NoOfSeedrs) s=1;
                         }

                       string addnl="";
                      
                  cout<<cmdStart<<">>>>"<<printEnd<<endl;
             cout<<endl<<oprStart<<"      Receiveing File........"<<printEnd<<endl;
             int stat=1,success=1,attempt=1;
                string dstFileName=destination_path+file_name;
              int FD = open(dstFileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
               while(success==1){
                     seederStatus[0]=1;
             int seedersSockets[NoOfSeedrs+2];
                  seedersSockets[0]=0;
                  for(int i=1;i<=NoOfSeedrs;i++)
                  {
                   vector<string> list=getCommands(seeders[i],',');
                    string chunksinfo="";
                    int flag=0;
                    for(int j=1;j<=NoOfChunks;j++){
                        if(getChunksfor[j]==i){
                            flag=1;
                            chunksinfo+=to_string(j)+":";
                        }
                    }
                    if(flag==0){
                        seederStatus[i]=1;
                        continue;
                    }
                      thread t(peer2peer,list[0],list[1],list[2],list[3],destination_path+file_name, chunksinfo,i,FD);
                        t.detach();
                  }
                     
                         int ff=1;
                         while(ff){
                            ff=0;
                            for(int k=1;k<=NoOfSeedrs;k++){
                                if(seederStatus[k]==0){
                                    ff=1;
                                }
                                else if(seederStatus[k]==-1){
                                    ff=0;
                                    stat=0;
                                    break;
                                }

                            }
                         }
                         attempt++;
                        if(stat==1) {
                              
                        vector<string>  getSHA1=cal_SHA(destination_path+file_name);

                          vector<string> listt=getCommands(seeders[1],',');
                        vector<string>  getOrgSHA1=cal_SHA(listt[1]);
                        success=0;
                        for (int ii = 0; ii < getOrgSHA1.size() && ii<getSHA1.size(); ii++) 
                        {
                            if (getSHA1[ii].compare(getOrgSHA1[ii]) !=0){
                               cout<<errorStart<<" SHA1 not matched for : "<<ii<<printEnd<<endl;
                               if(attempt<50){
                               success=1;
                               break;
                           }else{
                                 success=0;
                                 stat=0;
                                 break;
                           }
                                 
                        }
                    }
                   }

            }
                       if(stat==1) {
                        cout<<"  file integrity  Ensured with SHA1."<<endl;
                        cout<<oprStart<<"      File Downloaded  "<<printEnd<<endl;
                        string comm="upload_file";
                        upload_filefc(comm,dstFileName, group_id , c_IP, c_PORT, socketID);
                   }
                       else  cout<<errorStart<<"      File Download failed! please retry after sometime,,"<<printEnd<<endl;
                       for(int k=1;k<=NoOfSeedrs;k++)
                           seederStatus[k]=0;
                     D_files.erase(find(D_files.begin(),D_files.end(),file_name));
                     D_gid.erase(find(D_gid.begin(),D_gid.end(),group_id));
                     C_files.push_back(file_name);
                      C_gid.push_back(group_id);
                }
         }
         else if(command == "show_downloads"){
            cout<<">>>>"<<endl;
            if(D_files.size()==0){
           cout<<TrOpStart<<"    No files are Downloading"<<printEnd<<endl;
            }
            else{
                for (int i = 0; i <D_files.size(); i++) 
                        cout<<"     [D] [ "<<D_gid[i]<<" ] [ "<<D_files[i]<<" ] "<<endl;
            }
            if(C_files.size()==0){
           cout<<TrOpStart<<"    No files are Downloaded yet"<<printEnd<<endl;
            }
            else{
                for (int i = 0; i <C_files.size(); i++) 
                        cout<<"     [C] [ "<<C_gid[i]<<" ] [ "<<C_files[i]<<" ] "<<endl;
            }
            
         }
         else if(command == "stop_share"){
           string group_id,file_name;
           cin>>group_id>>file_name;
          string input=command+" "+group_id+" "+file_name;
           string op=getInfoFromTracker(input,socketID);
              if(op == "vailid")
                cout<<TrOpStart<<" Done.. "<<printEnd<<endl;
              else if(op == "invailid")
                cout<<errorStart<<"  Group doesn't exists : "<<group_id<<" "<<printEnd<<endl;
              else if(op == "nothing")
                cout<<errorStart<<"  file not shared before.. "<<printEnd<<endl;
              else 
                cout<<errorStart<<"  some error occured.. "<<printEnd<<endl;
         }
          else if(command == "logout"){
                 Session = 0;
           cout<<TrOpStart<<getInfoFromTracker(command,socketID)<<printEnd<<endl;
         }
         else cout<<errorStart<<"  invailid Input!"<<printEnd<<endl;
      }
      else  cout<<errorStart<<"  Please first login!"<<printEnd<<endl;
    }
    return 0;
}


//*********************************************| Testing |******************************************************************