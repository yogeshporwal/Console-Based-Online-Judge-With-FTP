/*  Name : Yogesh Porwal
    Roll No : 20CS60R52
    Email Id:yogeshporwal@kgpian.iitkgp.ac.in
    File name : client.cpp

    Compilation Instructions:
    To complie : g++ <source_filename> -o <any name>
                eg: g++ client.cpp -o client
    To Run : ./<any name*> <Hostname> <Port number>
            eg: ./client localhost 3000
            
            *this should be same as given while compiling tha file
*/

#include <unistd.h> 
#include <iostream> 
#include <sys/socket.h> 
#include<sys/types.h>
#include <stdlib.h> 
#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include <string.h> 
#include<sstream>
#include<fstream>
#include<dirent.h>

using namespace std;

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

/*  Function Prototype : int connect_to_server(string port)
    Input Arguments:  port no which will be used to connect server
    Description: connect to server on specified port no for data transfer 
*/

int connect_to_server(string port)
{
  struct sockaddr_in server_address,client_address;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);       //creating new socket

  if(sock_fd<0)
  {
    perror("socket creation for data connection failed!");
    exit(EXIT_FAILURE);
  }
  else
    cout<<"socket created for data connection successfuly!"<<endl; 

  int port_no=stoi(port);
  struct hostent *Server;
  Server = gethostbyname("localhost");
  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)Server->h_addr, (char *) &server_address.sin_addr.s_addr, Server->h_length);
  server_address.sin_port = htons(port_no);

  if(connect(sock_fd,(struct sockaddr *) &server_address,sizeof(server_address)) < 0)    // sending conection request to server
  { 
    perror("Error in connecting in data connection");
    exit(EXIT_FAILURE); 
  }
  else
    cout<<"connected to server successfuly for data connection!"<<endl;

  return sock_fd;
}

/*  Function Prototype : bool retrieve_file(string filename,int socket_fd)
    Input Arguments:  filename(string),socketfd
    Description:  implementation of RETR command of FTP at client side
*/

bool retrieve_file(string filename,int socket_fd)
{
  char buffer[1024];
  
  bzero(buffer,1024);
  int n = read(socket_fd,buffer,1024);

  if(n<=0)
  {
    cout<<"server is disconnected!"<<endl;
    return 0;
  }

  string port=convert_to_string(buffer);
  int new_fd=connect_to_server(port);     // creating new data connection
  cout<<"successfuly data connection established !"<<endl;
  
  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);

  if(buffer[0]=='1')
  {
    ofstream fout;
    fout.open(filename,ios::out|ios::binary);  //opening file in binary mode to write data
    
     cout<<"file is transferring..."<<endl;
    bzero(buffer,1024);
    while(read(new_fd,buffer,1023))   //reading data sent by server
    {
          
      fout.write((char *) &buffer, strlen(buffer)); //writing received data in file
      bzero(buffer,1024);
        
    }  
    cout<<"file successfully received from server"<<endl;
    fout.close();
  }
  else
  {
    cout<<"server replied: "<<buffer<<endl;
  }
  close(new_fd);
  cout<<"data connection closed!"<<endl;   
  return 1;
}

/*  Function Prototype : bool list_of_files(int socket_fd)
    Input Arguments:  socketfd of control connection
    Description:  implementation of LIST command of FTP at client side
*/

bool list_of_files(int socket_fd)
{
  char buffer[1024];
  bzero(buffer,1024);
  int n = read(socket_fd,buffer,1024);

  if(n<=0)
  {
    cout<<"server is disconnected!"<<endl;
    return 0;
  }

  string port=convert_to_string(buffer);
  int new_fd=connect_to_server(port);    // creating new data connection
  cout<<"successfuly data connection established !"<<endl;
  cout<<"reply from server :"<<endl;
  bzero(buffer,1024);
  while(read(new_fd,buffer,sizeof(buffer)))    //receiving filenames which are in server's current directory and printing them
  {
      cout<<buffer<<endl;   
      bzero(buffer,1024); 
  }
  close(new_fd);
  cout<<"data connection closed!"<<endl;
  return 1;
}

/*  Function Prototype : bool store_file(string filename,int socket_fd)
    Input Arguments:  filename(string),socketfd
    Description:  implementation of STOR command of FTP at client side
*/

bool store_file(string filename,int socket_fd)
{
  char buffer[1024];
  bzero(buffer,1024);
  int n = read(socket_fd,buffer,1024);

  if(n<=0)
  {
    cout<<"server is disconnected!"<<endl;
    return 0;
  }

  string port=convert_to_string(buffer);
  int new_fd=connect_to_server(port);
  cout<<"successfuly data connection established !"<<endl;

  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);
  
  if(buffer[0]=='1')
  {
  
    ifstream fin;
    fin.open(filename,ios::in|ios::binary);  //opening file in binary mode to read
    cout<<"file is transferring..."<<endl;
    while(!fin.eof())          // reading file and sending data to client
    {
      bzero(buffer,1024);
      fin.read((char *) &buffer, 1023);
      int n = write(new_fd,buffer,strlen(buffer)); 
    }

    fin.close();        //closing file
    cout<<"file successfuly sent to the server!"<<endl;
    
  }
  else
  {
    cout<<"server replied: "<<buffer<<endl;
  }
  close(new_fd);
  cout<<"data connection closed!"<<endl;
  return 1;
}

/*  Function Prototype : bool delete_file(string filename,int socket_fd)
    Input Arguments:  filename(string),socketfd of control connection
    Description:  implementation of DELE command of FTP at server side
*/

bool delete_file(string filename,int socket_fd)
{
  char buffer[1024];
  bzero(buffer,1024);
  int n = read(socket_fd,buffer,1024);

  if(n<=0)
  {
    cout<<"server is disconnected!"<<endl;
    return 0;
  }

  string port=convert_to_string(buffer);
  int new_fd=connect_to_server(port);     //connecting to server for new data connection
   cout<<"successfuly data connection established !"<<endl;
  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);
  cout<<"server replied: "<<buffer<<endl;
  close(new_fd);
  cout<<"data connection closed!"<<endl;
  return 1;
}

/*  Function Prototype : bool codejudge(string filename,string ext,int new_sockfd)
    Input Arguments: filename(string),extension of that file,socketfd of control connection
    Description:  implementation of CODEJUD command(of client side) is there in this function
                  
*/

bool codejudge(string filename,string ext,int new_sockfd)
{
  char buffer[1024];
  bzero(buffer,1024);
  int n = read(new_sockfd,buffer,1024);

  if(n<=0)
  {
    cout<<"server is disconnected!"<<endl;
    return 0;
  }

  string port=convert_to_string(buffer);
  int new_fd=connect_to_server(port);
  cout<<"successfuly data connection established !"<<endl;

  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);

  if(buffer[0]!='~')
  {
    cout<<"server replied: "<<buffer<<endl;
    close(new_fd);
    cout<<"data connection closed!"<<endl;
    return 1;
  }

  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);

  if(buffer[0]!='1')
  {
    cout<<"server replied: "<<buffer<<endl;
    close(new_fd);
    cout<<"data connection closed!"<<endl;
    return 1;
  }

  bzero(buffer,1024);
  n = read(new_fd,buffer,1024);

  if(buffer[0]=='1')
  {
    bzero(buffer,1024);
    n = read(new_fd,buffer,1024);
    cout<<"server replied: "<<buffer<<endl;

    bzero(buffer,1024);
    n = read(new_fd,buffer,1024);

    if(buffer[0]!='1')
    {
      cout<<"server replied: "<<buffer<<endl;
      close(new_fd);
      cout<<"data connection closed!"<<endl;
      return 1;
    }
    
    bzero(buffer,1024);
    n = read(new_fd,buffer,1024);
    cout<<"server replied: "<<buffer<<endl;
    int x=1;
    while(1)
    {
      if(buffer[0]=='~')
      {
        close(new_fd);
        cout<<"data connection closed!"<<endl;
        x=0;
        return 1;
      }
      
      if(x)
      {
        bzero(buffer,1024);
        n = read(new_fd,buffer,1024);
        cout<<"server replied: "<<buffer<<endl;
      }
      
    }


  }
  else
  {
    cout<<"server replied: "<<buffer<<endl;
    close(new_fd);
    cout<<"data connection closed!"<<endl;
    return 1;
  }
  return 1;
}

/*  Function Prototype : int main(int args,char* argv[])
    Input :         host name followed by port number as acommand line arguement
    Description:    code to set up connection with server is written,it is a continous program that contously take
                    query from client and server return appropriate response to that
*/

int main(int args,char* argv[])
{
 
  struct sockaddr_in server_addr,client_addr;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);   //socket creation
 
  if(socket_fd<0)
  {
    perror("socket creation failed!");
    exit(EXIT_FAILURE);
  }
  else
    cout<<"socket created successfully!"<<endl;
 
  if(argv[1]==NULL)
  {
    cout<<"Please pass the hostname"<<endl;
    return 0;
  }
 
 if(argv[2]==NULL)
 {
  cout<<"Please pass the port number"<<endl;
  return 0;
 }

  int port_no=stoi(argv[2]);
  struct hostent *server;
  server = gethostbyname(argv[1]);
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
  server_addr.sin_port = htons(port_no);
 
  if(connect(socket_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0)   // connecting to server for control connection 
  { 
    perror("Error in connecting");
    exit(EXIT_FAILURE); 
  }
  else
    cout<<"connected to server successfuly!"<<endl;

  cout<<"control connection established !"<<endl;
 
  char buffer[1024];
  int n,m;

  while(1)
  {
    bzero(buffer,1024);
    printf("\nEnter a command for server: "); 
    if(fgets(buffer,1024,stdin)==NULL)
    continue;
    
    string s=convert_to_string(buffer);

    stringstream ss(s); string command;
    ss>>command;

    if(command=="RETR")               // if command is RETR
    {
    
      string filename;
      ss>>filename;
      if(filename.length()==0)                   //if filename not given
      {
        cout<<"filename is not provided,please re-enter the command"<<endl;
        continue;
      }
      
      if(does_file_exist(filename))               //if file already exist
      {
        cout<<"file with the same name already exist in current directory!"<<endl;
        cout<<"enter 'Y' to overwrite the file or enter any other key to cancel the operation:"<<endl;
        string s;
        getline(cin,s);

        if(s!="Y")
        continue;
      }
      n=write(socket_fd, buffer, strlen(buffer));
      bool x=retrieve_file(filename,socket_fd);   

      if(!x)
      {
        cout<<"client is disconnecting...!"<<endl;
        break;
      }   

    }
    else if(command=="LIST")     // if command is LIST
    {
      n=write(socket_fd, buffer, strlen(buffer)); 
      bool x=list_of_files(socket_fd);

      if(!x)
      {
        cout<<"client is disconnecting...!"<<endl;
        break;
      }   
    }
    else if(command=="STOR")    // if command is STOR
    {
      string filename;
      ss>>filename;


      if(filename.length()==0)    //if filename not provided
      {
        cout<<"filename is not provided,please re-enter the command"<<endl;
        continue;
      }

      if(!does_file_exist(filename))       //if file doesn,t exist
      {
        cout<<"file to be sent ,doesn't exist in current directory,please give command with correct filename"<<endl;
        continue;
      }
      n=write(socket_fd, buffer, strlen(buffer)); 
      bool x=store_file(filename,socket_fd);

      if(!x)
      {
        cout<<"client is disconnecting...!"<<endl;
        break;
      }   
    }
    else if(command=="DELE")       // if command is DELE
    {
      string filename;
      ss>>filename;

      if(filename.length()==0)       //if filename not provided
      {
        cout<<"filename is not provided,please re-enter the command"<<endl;
        continue;
      }
      n=write(socket_fd, buffer, strlen(buffer)); 
      bool x=delete_file(filename,socket_fd);

      if(!x)
      {
        cout<<"client is disconnecting...!"<<endl;
        break;
      }   
    }
    else if(command=="QUIT")   // if command is QUIT
    {
      n=write(socket_fd, buffer, strlen(buffer)); 
      cout<<"client is disconnecting...!"<<endl;
      break;
    }
    else if(command=="CODEJUD")
    {
      string filename;
      ss>>filename;

      if(filename.length()==0)    //if filename not provided
      {
        cout<<"filename is not provided,please re-enter the command"<<endl;
        continue;
      }

      string ext;
      ss>>ext;

      if(ext.length()==0)    //if filename not provided
      {
        cout<<"extension of file is not provided,please re-enter the command"<<endl;
        continue;
      }

      n=write(socket_fd, buffer, strlen(buffer)); 
      bool x=codejudge(filename,ext,socket_fd);

      if(!x)
      {
        cout<<"client is disconnecting...!"<<endl;
        break;
      }   
    }
    else if(command=="ABOR")
    {
      cout<<"ABOR command is not supported!"<<endl;
    }
    else
    {
      cout<<"please enter correct command!"<<endl;
    }


 
  }
 
 
 return 0;
}