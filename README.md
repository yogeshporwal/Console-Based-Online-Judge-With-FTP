# Console-Based-Online-Judge-With-FTP

## Project Description

I believe most of the people must be aware of the term "Online Judge" or "OJ" because in any onliine coding platform(e.g. leetode,GFG etc.) people needs to interact with it. Basically OJ take codes , compile it, check for compilation errors, execute your code by providing some secret inputs and collect your code's output, check for runtime errors(or TLE),and finally match your output with its hidden test outputs.

In this project I have implemented Console based Online Judge in which there will be a server and multiple clients communicating with the server.Each client can send a c/c++ file to the server through CODEJUD command and the server will reply to the client whether the given code is successful or giving error(like compilation,runtime,TLE etc) at any point of time during execution of the code. If execution is successful then the server will also check and reply to the client about the acceptance of the c/c++ file.

In order to complete above task I have also implemented file transfer protocol(FTP) on top of the TCP sockets.There will be a server and multiple clients communicating with the server. Each client process will open a new connection with the server. I have used "select" system call to handle multiple client requests.  

I have implemented following FTP commands and "CODEJUD" command that a client can use,they work as follows:

- **RETR :**  This command causes the remote host to initiate a data connection and to send the requested file over the data connection.
- **STOR :** This command causes to store a file into the current directory of the remote host.
- **LIST :** Sends a request to display the list of all files present in the directory.
- **QUIT :** This command terminates a USER and if file transfer is not in progress , the server closes the control connection.
- **DELE :** This command deletes a file in the current directory of server.
- **CODEJUD :** This command will take a c/c++ file from client and server will compile , execute and match the output with given test cases and will notify back to client about any error or correctness of c/c++ file.

## Instructions to run the code

* To judge any file using "CODEJUD" command there must be a file at server side to match output for that program otherwise it will give an error, so first use "STOR" command to send standard testcase output file to server. For runtime input, input text file should also be sent using "STOR" command priorly.Also the program we are going to check should also be there at server side.
* Naming convention of standard testcase files and input files must be as follows:

  - **Input file:** input_(program name).txt e.g. if program file is greetings.cpp then input file should be named as "input_greetings.txt" 
  - **Standard Output File:** testcase_(program name).txt e.g. if program file is greetings.cpp then standard output file should be named as testcase_greetings.txt"   
  **Note:** you can refer "Smple Files" folder of this repo. to know more.
  
* You can enter any of the following comand in below format:
  - RETR \<filename>
  - STOR \<filename>
  - LIST
  - QUIT
  - DELE \<filename>
  - CODEJUD \<filename> <ext(c/c++)>

### commands to run code:

- For server file: 

  To complie : ```$ g++ <source_filename> -o <any name> 
                eg: $ g++ server.cpp -o server ```
                
  To Run : ``` $ ./<any name*> <Port number>
              eg: $ ./server 8000 ```

              *this should be same as given while compiling that file

              Please press ctrl+c to terminate server.
  
 - For client file: 

    For  complie : ```$ g++ <source_filename> -o <any name>
                    eg:$ g++ client.cpp -o client ```
                
    To Run : ```$ ./<any name*> <host name> <Port number>
                eg:$ ./client localhost 8000 ```

              *this should be same as given while compiling that file




