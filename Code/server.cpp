/*  Name : Yogesh Porwal
    Roll No : 20CS60R52
    Email Id:yogeshporwal@kgpian.iitkgp.ac.in
    File name : server.cpp

    Compilation Instructions:
    To complie : g++ <source_filename> -o <any name>
                eg: g++ server.cpp -o server
    To Run : ./<any name*> <Port number>
            eg: ./server 3000
            
            *this should be same as given while compiling tha file
*/

#include<iostream>
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <string.h> 
#include<sstream>
#include<math.h>
#include <fstream> 
#include<ctime>
#include<netdb.h>
#include<dirent.h>
#include<ctime>

using namespace std;

struct connection_info  // instance of this structure will contain port number and socket id
{                      //  for each new data connection
  int port_no;
  int newsocket;
};

struct node           // structure of node of linklist,to store reserved port number
{
  int port_no;
  node * next;
}*head=NULL;

/*  Function Prototype : void add_node(int port) 
    Input :         port number to be added in linklist_of_files(as a node)
    Description:    add new node in lintlist containing port number(which is passes)
*/

void add_node(int port)   
{
  node *t=(node *)malloc(sizeof(node));
  t->port_no=port;
  t->next=head;
  head=t; 
}

/*  Function Prototype : void delete_node(int port)
    Input :         port number to be deleted from linklist
    Description:    node containing passed port no will be deleted
*/

void delete_node(int port)
{
  
  node *temp=head;
  if(head->port_no==port)
  {
    head=head->next;
    free(temp);
    return;
  }

  node* prev=NULL;
  while(temp)
  {
    if(temp->port_no==port)
    {
      break;
    }
    prev=temp;
    temp=temp->next;
  }

  prev->next=temp->next;
  free(temp);
}

/*  Function Prototype: void send_msg(string s,int &new_sockfd)
    Input :         message to be sent to client as a string,reference to socket id of client
    Description:       passed message send to the client
*/

void send_msg(string s,int &new_sockfd)
{
  char message[1024];
  bzero(message,1024);
  strcpy(message, s.c_str());
  int n = write(new_sockfd,message,1024); 
}

/*  Function Prototype : string convert_to_string(char array[])
    Input Arguments:     char array
    Return :         converted string 
    Description:    convert char array to string data type
*/

string convert_to_string(char array[])
{
    string s;
    for(int i=0;i<strlen(array);i++)
    {
      s+=array[i];
    }  

    return s;
}

/*  Function Prototype : bool does_file_exist(string filename)
    Input Arguments:     filename(string)
    Return :         1 if file exist in current directory otherwise 0
    Description:  check if given file exist in current directory
*/

bool does_file_exist(string filename)
{
  struct dirent *dir;
  DIR *open = opendir(".");  

  while ((dir = readdir(open)) != NULL) 
  {
    string name=convert_to_string(dir->d_name);
    
    if(name==filename)
    {
      closedir(open); 
      return 1;  
    }
    
  }  
  
  closedir(open);  
  return 0;
}

/*  Function Prototype : void reserve_port(int port_no)
    Input Arguments:     port no to be reserved
    Description:  reserve the given port number,so server not use for data connecton while it is being used
*/

void reserve_port(int port_no)
{
  add_node(port_no);
} 


/*  Function Prototype : void free_port(int port_no)
    Input Arguments:     port no to be released
    Description: release the reserved port no when data connection closing,so that other connection can be established using this
*/

void free_port(int port_no)
{
  delete_node(port_no);
}

/*  Function Prototype : bool isAvailable(int port)
    Input Arguments:     port number
    Return :         1 if given port no availabe for use otherwise 0
    Description:  check if given port no is used by server for other connection
*/

bool isAvailable(int port)
{
  node *temp=head;

  while(temp)
  {
    if(temp->port_no==port)
      return 0;

    temp=temp->next;
  }
  return 1;
}

/*  Function Prototype : int generate_port()
    Input Arguments:   nothing
    Return :   return a unique port no that is not being used for any connection
    Description:  generate a random port no in valid range,which can be used
*/

int generate_port()
{
  int new_port;
  srand(time(0));
  while(1)
  {
    new_port=1024+rand()%60000;

    if(isAvailable(new_port))
    {
      break;
    }
  }

  return new_port;
}

/*  Function Prototype : void setDataConnection(int new_sockfd,connection_info &My)
    Input Arguments:  socketfd and reference of the instance of the connection_info struct
    Description: establish new data connection with the clientfor data transfer 
*/

void setDataConnection(int new_sockfd,connection_info &My)
{
  int port_no=generate_port();
  reserve_port(port_no);
  My.port_no=port_no;
  string port=to_string(My.port_no);
  

 struct sockaddr_in server_address,client_address; 
 bzero((char *) &server_address, sizeof(server_address)); 
 
 server_address.sin_family = AF_INET; 
 server_address.sin_addr.s_addr = INADDR_ANY; 
 server_address.sin_port = htons(port_no); 
 
 int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
 
 if(sock_fd<0)
 {
   perror("socket creation for data connection failed!");
   exit(EXIT_FAILURE);
 }
 else
 cout<<"socket for data connection created successfuly!"<<endl;
 
 
 
 if(bind(sock_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
 {
  perror("binding failed for data connection,Error!");
  exit(EXIT_FAILURE);
 }
 else
 cout<<"binding done for data connection"<<endl;
 
 
 if (listen(sock_fd, 5) < 0) 
 { 
  perror("Error on listen in data connection"); 
  exit(EXIT_FAILURE); 
 } 
 else
 cout<<"Listening..."<<endl;
 
 socklen_t client_length=sizeof(client_address);
 send_msg(port,new_sockfd);
 int new_fd = accept(sock_fd, (struct sockaddr *) &client_address,&client_length);
 My.newsocket=new_fd;

}

/*  Function Prototype : void retrieve_file(string filename,int &new_sockfd)
    Input Arguments:  filename(string),socketfd
    Description:  implementation of RETR command of FTP at server side
*/

void retrieve_file(string filename,int &new_sockfd)
{
  connection_info My;
  setDataConnection(new_sockfd,My);
  cout<<"successfuly data connection established !"<<endl;
  char buffer[1024];
  if(!does_file_exist(filename))       //if file doesn't exist
  {
    cout<<"Requested file doesn't exist !"<<endl;
    send_msg("Requested file doesn't exist!",My.newsocket);
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
    return ;
  }

  ifstream fin;
  fin.open(filename,ios::in|ios::binary); //opening file in binary mode to read data
    send_msg("1",My.newsocket);  

    cout<<"file is transferring..."<<endl;
    while(!fin.eof())          // reading file and sending data to client
    {
    bzero(buffer,1024);
    fin.read((char *) &buffer, 1023);
    int n = write(My.newsocket,buffer,strlen(buffer)); 
    }
  
  cout<<"file successfuly sent to the client!"<<endl;
  fin.close();                   //closing file
  close(My.newsocket);     
  cout<<"data connection closed!"<<endl;
 
}

/*  Function Prototype : void store_file(string filename,int &new_sockfd)
    Input Arguments:  filename(string),socketfd
    Description:  implementation of STOR command of FTP at server side
*/

void store_file(string filename,int &new_sockfd)
{
  connection_info My;
  setDataConnection(new_sockfd,My);
  cout<<"successfuly data connection established !"<<endl;
  char buffer[1024];
  if(does_file_exist(filename))          //if file already exist
  {
    cout<<"file with the same name already exist in current directory"<<endl;
    send_msg("file with the same name already exist in current directory",My.newsocket);
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
    return;
  }

  send_msg("1",My.newsocket);  

  ofstream fout;
  fout.open(filename,ios::out|ios::binary); //opening file in binary mode to write

  bzero(buffer,1024);
  cout<<"file is transferring..."<<endl;
  while(read(My.newsocket,buffer,1023))      //reading data from client and storing in file
  {
             
    fout.write((char *) &buffer, strlen(buffer));
    bzero(buffer,1024);
            
  }  
  
  cout<<"file succefully stored in current directory!"<<endl;
  fout.close();
  close(My.newsocket);
  cout<<"data connection closed!"<<endl;
}

/*  Function Prototype : void list_of_files(int &new_sockfd)
    Input Arguments:  socketfd of control connection
    Description:  implementation of LIST command of FTP at server side
*/

void list_of_files(int &new_sockfd)
{
  connection_info My;
  setDataConnection(new_sockfd,My);
  cout<<"successfuly data connection established !"<<endl;
  struct dirent *dir;
  DIR *open = opendir("."); 

  if (open == NULL)  // opendir returns NULL if couldn't open directory 
  { 
    cout<<"could not open current directory"<<endl;
    close(My.newsocket);
    return;
  } 

  while ((dir = readdir(open)) != NULL)   
  {
    string name=convert_to_string(dir->d_name);
    if(name=="." || name=="..")
    continue;

    send_msg(name,My.newsocket);  //sending file names to client
  }  
  cout<<"Response sent to the client!"<<endl;
    closedir(open);  
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
}

/*  Function Prototype : void delete_file(string filename,int &new_sockfd)
    Input Arguments:  filename(string),socketfd of control connection
    Description:  implementation of DELE command of FTP at server side
*/

void delete_file(string filename,int &new_sockfd)
{
  connection_info My;
  setDataConnection(new_sockfd,My);
  cout<<"successfuly data connection established !"<<endl;

  if(!does_file_exist(filename))  //if file not exist
  {
    cout<<"file to be deleted does not exist in current directory"<<endl;
    send_msg("file to be deleted does not exist in current directory",My.newsocket);
    cout<<"Response sent to the client!"<<endl;

  }
  else
  { char name[500];
    bzero(name,500);
    strcpy(name, filename.c_str());
    int x=remove(name);              //deleting file
    if(!x)
    {
      cout<<"file successfuly deleted!"<<endl;
      send_msg("file successfuly deleted!",My.newsocket);
      cout<<"Response sent to the client!"<<endl;
    }
    else
    {
      cout<<"error occured while deleting!"<<endl;
      send_msg("error occured while deleting!",My.newsocket);
      cout<<"Response sent to the client!"<<endl;
    }
  }

  close(My.newsocket);
  cout<<"data connection closed!"<<endl;

}

/*  Function Prototype : int file_match(string file1,string file2,int client_fd)
    Input Arguments:  name of file1(string),name of file2(string),socketfd of client
    Description:  it takes two files and match content line by line,and send appropriate msg for 
                  matching of each lineS
*/

int file_match(string file1,string file2,int client_fd)
{
  if(!does_file_exist(file1))
  {
    return 2;
  }
  
  ifstream fin1,fin2;  
  fin1.open(file1);      //opening files
  fin2.open(file2);
  int line_no=0, flag=1;
  string line1,line2;
  
  while(!fin1.eof() && !fin2.eof())       //while pointer does not reach to the end of any file
  {
    line_no++;
    getline(fin2,line2);
  
    if(line2[line2.length()-1]=='\n' || line2[line2.length()-1]=='\r')
    line2=line2.substr(0,line2.length()-1);

    getline(fin1,line1);
    if(line1[line1.length()-1]=='\n' || line1[line1.length()-1]=='\r')
    line1=line1.substr(0,line1.length()-1);
    
    string lno = to_string(line_no);

    if(line1.size()==0 && line2.size()==0)
    {
      break;
    }

    if(line1!=line2)            // if lines don't match
    {
      flag=0;
      cout<<"Testcase-"<<line_no<<" failed!"<<endl;
      send_msg("Testcase-"+lno+" failed!",client_fd);
    }
    else
    {
      cout<<"Testcase-"<<line_no<<" passed..."<<endl;
      send_msg("Testcase-"+lno+" passed...",client_fd);
    }
  }

  while(! fin1.eof())            
  {
    getline(fin1,line1);
    if(line1.size()==0 && line2.size()==0)
    {
      break;
    }
    flag=0;
    cout<<"Testcase-"<<line_no<<" failed!"<<endl;
    send_msg("Testcase-"+to_string(line_no)+" failed!",client_fd);
  }

  if(flag)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


/*  Function Prototype : void codejudge(string filename,string ext,int new_sockfd)
    Input Arguments: filename(string),extension of that file,socketfd of control connection
    Description:  implementation of CODEJUD command is there in this function,it takes file(c or c++),
                  compile,run that file using system() ,and then match with correct outputs,and sent
                  appropriate message accordingly 
*/

void codejudge(string filename,string ext,int new_sockfd)
{
  connection_info My;
  setDataConnection(new_sockfd,My);  
  cout<<"successfuly data connection established !"<<endl;

  char buffer[512];
  string command;
  if(ext=="c")
  {
    command="gcc ";
    string real_ext=filename.substr(filename.length()-1,1);

    if(real_ext!="c")                      //matching if passed extension and the extension file has are same or not
    {
      cout<<" file with the wrong extension is provided! ,it should be ."<<ext<<endl;
      send_msg(" file with the wrong extension is provided! ,it should be ."+ext,My.newsocket);
      cout<<"Response sent to the client!"<<endl;
      close(My.newsocket);
      cout<<"data connection closed!"<<endl;
      return;
    }
    else
    {
      send_msg("~",My.newsocket);
      filename.erase(filename.end()-2,filename.end());
      ext=".c";
    }
  
  }
  else if(ext=="c++" || ext=="cpp")    //matching if passed extension and the extension file has are same or not
  {
    command="g++ ";
    string real_ext=filename.substr(filename.length()-3,3);

    if(real_ext!="cpp" && real_ext!="c++")
    {
      cout<<" file with the wrong extension is provided! ,it should be ."<<ext<<endl;
      send_msg(" file with the wrong extension is provided! ,it should be ."+ext,My.newsocket);
      cout<<"Response sent to the client!"<<endl;
      close(My.newsocket);
      cout<<"data connection closed!"<<endl;
      return;
    }
    else
    {
      send_msg("~",My.newsocket);
      filename.erase(filename.end()-4,filename.end());
      ext=".cpp";
    }
    
  }
  else
  {
    cout<<"."<<ext<<" files are not supported!"<<endl;     // if files are not c/c++ type
    send_msg("."+ext+" files are not supported!",My.newsocket);
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
    return;
  }

  if(!does_file_exist(filename+ext))  //if program file does not exist in current directory
  {
    cout<<filename<<ext<<" does not exist in current directory"<<endl;
    send_msg(filename+ext+" does not exist in current directory,please enter command with correct filename",My.newsocket);
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
    return;
  }
  else
  {
    send_msg("1",My.newsocket);
  }

  command+=filename+ext+" -o "+filename + " 2>error";
  bzero(buffer,511);
  strcpy(buffer, command.c_str());
  if(!system(buffer))       // compiling
  {
    cout<<"compiled successfully!"<<endl;
    send_msg("1",My.newsocket);
    send_msg("compiled successfully!",My.newsocket);
    ext=".txt";
    
    time_t start,end;
    ifstream fin;

    if(does_file_exist("input_"+filename+ext))  // if program requires some input from file
    {
      fin.open("input_"+filename+ext);
      command="timeout 2 ./"+filename+"< dummy_input_"+filename+ext+" > dummy_output_"+filename+ext;

      ofstream newfile;
      newfile.open("output_"+filename+ext);
    
      bzero(buffer,511);
      strcpy(buffer, command.c_str());
      
      while(!fin.eof())      //while input file has testcase(s) to be run
      {
        ofstream fout;
        fout.open("dummy_input_"+filename+ext);
        string line;
        getline(fin,line);
        fout<<line;
        fout.close();
        time(&start);
        if(!system(buffer))       // runnning the program
        {
          time(&end);
          double execution_time=1.0 *(end-start);
        if(execution_time>1.0)                    // if TLE 1sec
        {
          cout<<"Time Limit Exceed(runtime) error!"<<endl;  
          send_msg("Time Limit Exceed(runtime) error!",My.newsocket); 
          cout<<"Response sent to the client!"<<endl;
          close(My.newsocket);
          cout<<"data connection closed!"<<endl;
          return;
        }

        ifstream new_in;
        new_in.open("dummy_output_"+filename+ext);
        getline(new_in,line);
        newfile<<line<<endl;
        new_in.close();
        char name[500];
        string s="dummy_output_"+filename+ext;
        strcpy(name,s.c_str());
        remove(name);
        s="dummy_input_"+filename+ext;
        strcpy(name,s.c_str());
        remove(name);

      }
      else
      {
        time(&end);
        double execution_time=1.0 *(end-start);
        if(execution_time>1.0)
        {
          cout<<"Time Limit Exceed(runtime) error!"<<endl;  
          send_msg("Time Limit Exceed(runtime) error!",My.newsocket); 
          cout<<"Response sent to the client!"<<endl;
          close(My.newsocket);
          cout<<"data connection closed!"<<endl;
          return;
        }
         
        cout<<"runtime error!"<<endl;    // if runtime error
        send_msg("runtime error!",My.newsocket); 
        cout<<"Response sent to the client!"<<endl;
        close(My.newsocket);
        cout<<"data connection closed!"<<endl;
        return;
      }

    }
  
    newfile.close();
    }
    else
    {
      command="timeout 2 ./"+filename+" > output_"+filename+ext;

      bzero(buffer,511);
      strcpy(buffer, command.c_str());

      time(&start);
      if(!system(buffer))   //running the program
      {
        time(&end);
        double execution_time=1.0 *(end-start);
        if(execution_time>1.0)                  // if TLE 1 sec
        {
          cout<<"Time Limit Exceed(runtime) error!"<<endl;  
          send_msg("Time Limit Exceed(runtime) error!",My.newsocket); 
          cout<<"Response sent to the client!"<<endl;
          close(My.newsocket);
          cout<<"data connection closed!"<<endl;
          return;
        }

      }
      else
      {
        time(&end);
        double execution_time=1.0 *(end-start);
        if(execution_time>1.0)
        {
          cout<<"Time Limit Exceed(runtime) error!"<<endl;  
          send_msg("Time Limit Exceed(runtime) error!",My.newsocket); 
          cout<<"Response sent to the client!"<<endl;
          close(My.newsocket);
          cout<<"data connection closed!"<<endl;
          return;
        }
         
        cout<<"runtime error!"<<endl;    // if runtime error occured
        send_msg("runtime error!",My.newsocket); 
        cout<<"Response sent to the client!"<<endl;
        close(My.newsocket);
        cout<<"data connection closed!"<<endl;
        return;
      }
    }
    send_msg("1",My.newsocket); 
    
    cout<<"program successfully executed!"<<endl;  
    send_msg("program successfully executed!",My.newsocket);
    
    int val=file_match("testcase_"+filename+ext,"output_"+filename+ext,My.newsocket);
         
    if(val==1)  
    {
      cout<<"* * * SUCCESS! * * *"<<endl;  
      cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
      send_msg("* * * SUCCESS! * * *",My.newsocket);
    }
    else if(val==0)
    {
      cout<<"* * * WRONG OUTPUT! * * *"<<endl;  
      send_msg("* * * WRONG OUTPUT! * * *",My.newsocket);
      cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    }
    else
    {
      cout<<"Testcase file for given program is missing!"<<endl;  
      send_msg("Testcase file for given program is missing!",My.newsocket);
      cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    }
    send_msg("~~~~~~~~~~~~~~~~~~~~~~~~~",My.newsocket);
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
  }
  else
  {
    cout<<"compilation error!"<<endl;   
    send_msg("compilation error!",My.newsocket); 
    cout<<"Response sent to the client!"<<endl;
    close(My.newsocket);
    cout<<"data connection closed!"<<endl;
    return;
  }
   

}

/*  Function Prototype : int main(int args,char* argv[])
    Input :         port number as acommand line arguement
    Description:    FTP protocol for multi-client is implemented using select system call
*/

int main(int argc , char *argv[]) 
{ 

int my_socket=socket(AF_INET , SOCK_STREAM , 0);

if(my_socket<0)
{
  perror("socket creation failed!");
  exit(EXIT_FAILURE);
}
else
{
  cout<<"socket created successfully!"<<endl;
}
  

struct sockaddr_in server_addr;
 
if(argv[1]==NULL)
{
  cout<<"Please pass the port number"<<endl;
  return 0;
}

int PORT=stoi(argv[1]);  
reserve_port(PORT);                // reserving port no
server_addr.sin_family = AF_INET; 
server_addr.sin_addr.s_addr = INADDR_ANY; 
server_addr.sin_port = htons( PORT );  
	
memset(&(server_addr.sin_zero),0,8);
int port_no[100];
memset(port_no,0,sizeof(port_no));
	
if(bind(my_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)  //binding
{
  perror("binding failed,Error!");
  exit(EXIT_FAILURE);
}
else
  cout<<"binding done "<<endl;
	
if (listen(my_socket, 5) < 0) 
{ 
  perror("Error on listen"); 
  exit(EXIT_FAILURE); 
} 
else
  cout<<"Listening..."<<endl;
	
fd_set curr_fd,ready_fd;
FD_ZERO(&curr_fd);
FD_ZERO(&ready_fd);
FD_SET(my_socket, &curr_fd); 
	
struct sockaddr_in client_addr;
socklen_t client_length;
client_length=sizeof(client_addr); 

while(1)
{
    ready_fd=curr_fd;
    int activity=select( FD_SETSIZE, &ready_fd , NULL , NULL , NULL); // select system call
	 
    if(activity<0)
    {
      perror("select");
      exit(EXIT_FAILURE);
    }
	
	for(int k=0;k<FD_SETSIZE;k++)	
    {
        if(FD_ISSET(k,&ready_fd))
        {
            if(k==my_socket)
            {
              int new_socket = accept(my_socket, (struct sockaddr *)&client_addr, &client_length);
            
            if(new_socket<0)
            {
              cout<<"couldnt accept the connection";
              exit(EXIT_FAILURE);
            }
            else
            {
              cout<<endl<<"control connection established with the new client "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
              reserve_port(ntohs(client_addr.sin_port)); 
            }
            
            FD_SET(new_socket,&curr_fd); 
            port_no[new_socket]=ntohs(client_addr.sin_port);
            }
            else
            {
              char buffer[1024];
              memset(&(buffer),0,1024);
              int n=read(k,buffer,1024);

              if(n<1)
              continue;

              string s;
              s=convert_to_string(buffer);

              stringstream ss(s); string command;
              ss>>command;

              cout<<endl<<command<<" command received from client "<<inet_ntoa(client_addr.sin_addr)<<":"<<port_no[k]<<endl;

              if( strncmp("QUIT", buffer, 3)==0)   // if command is QUIT
              {
                cout<<endl<<"client "<<inet_ntoa(client_addr.sin_addr)<<":"<<port_no[k]<<" is disconnecting!"<<endl;
                FD_CLR(k,&curr_fd);
                continue;
              }

              
              if(command=="RETR")     // if command is RETR
              {
          
                string filename;
                ss>>filename;
                if(fork()==0)
                {
                  retrieve_file(filename,k);
                  return 0;
                }
                
                
              }
              else if(command=="LIST")  // if command is LIST
              {
                
                if(fork()==0)
                {
                  list_of_files(k);
                  return 0;
                }
              }
              else if(command=="STOR")  // if command is STOR
              {
                string filename;
                ss>>filename;
                if(fork()==0)
                {
                  store_file(filename,k);
                  return 0;
                }
                
                
              }
              else if(command=="DELE")  // if command is DELE
              {
                string filename;
                ss>>filename;
                if(fork()==0)
                {
                  delete_file(filename,k);
                  return 0;
                }
                
              }
              else if(command=="CODEJUD")  //if command is CODEJUD
              {
                string filename;
                ss>>filename;
                string ext;
                ss>>ext;

                if(fork()==0)
                {
                  codejudge(filename,ext,k);
                  return 0;
                }

              }
              else
              {

              }
              




            }
        }
    }
         
} 
close(my_socket);	 
return 0;
}

