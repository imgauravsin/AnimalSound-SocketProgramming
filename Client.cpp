#include <iostream>
#include <sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include<vector>

#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<string.h>
#include<unordered_map>
#include<algorithm>

#define SA struct sockaddr
#define MAX 1024

using namespace std;
int main()
{
    int sockFd,loginStats=-1,stdineof,maxfd;
    fd_set rset;
    struct sockaddr_in servaddr;
    char buff[MAX];

     
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
     if(sockFd == -1)
    {
        cout<<"\nSocket creation failed...\n";
        exit(0);
    } 
    else
        cout<<"\nSocket successfully created :)\n\n";

     
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port =htons(8080);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =INADDR_ANY;
     
    
    if(connect(sockFd, (SA*) &servaddr, sizeof(servaddr)) != 0)
    {
        cout<<"Connection with the server failed...\n";
        exit(0);
    }
    else
        cout<<"Connected to the server Successfully! \n";
    
    //----------------------------------------------------------------------------
    
    FD_ZERO(&rset);
    stdineof=0;
    int n;

	 
	
	cout<<"\nLogin With Your Entry : \n\n";
	
	write(1,"UserId : ",sizeof("UserId : "));

    for( ; ; )
    {
		
		FD_SET(fileno(stdin),&rset);  

        FD_SET(sockFd,&rset);  
        
        maxfd = std::max(fileno(stdin),sockFd) + 1;
		
		select(maxfd , &rset , NULL, NULL , NULL);
        
		 
        if(FD_ISSET(fileno(stdin),&rset))
        {   

		 

			if(loginStats < 0)
			{   
				string id,password,login="";
				
				getline(cin,id);
				
			 
				write(1,"Password : ",sizeof("Password : "));

				getline(cin,password);
 
				if(id.size() == 0 || password.size() == 0)
				{
					write(1,"\nUserId : ",sizeof("\nUserId : "));
					continue;
				}


				login+=id+" "+password;
				
				std::copy(login.begin(),login.end(),buff);

				buff[login.size()]='\0';        

				 
				write(sockFd,buff,sizeof(buff));
		
			}
        	
			 
			else
			{	

				bzero(buff,MAX);

 				string userIp;

 				cin.getline(buff,sizeof(buff));
 
				userIp=buff;
				
				//if empty request
				if(userIp.size() ==0 ) 
				{	
					write(1,"\n $ : ",sizeof("\n$ : "));
					continue;
				}

 				std::transform(userIp.begin(),userIp.end(),userIp.begin(),::tolower);

 				if(userIp.compare("store")==0)
				{   
					string animalName,animalSound,storeResult="";
					
					cout<<"Enter Animal Name : ";
					getline(cin,animalName);

					cout<<"Enter Animal Sound : ";
					getline(cin,animalSound);

 
					storeResult=userIp+" "+animalName+" "+animalSound;
					std::copy(storeResult.begin(),storeResult.end(),buff);

				}       

 				write(sockFd,buff,sizeof(buff));
				
			}

		}
        
		 

 
        if(FD_ISSET(sockFd,&rset))
        {	
             if(loginStats < 0)
            {
                int status=0;
                n = read(sockFd, &status, sizeof(status));
                
				/*status=1 : NEW USER 
				  status=2 : OLD USER   
				  status=3 : INCORRECT DETAILS 
				*/
                
				//1
                if(ntohl(status)==1)
                {
                    cout<<"\n***** Welcome to the Animal Sound Server *****\n\nYou are successfully Registered  \n";
                  
                     
                    loginStats=1;
                }
				
				//2
                else if(ntohl(status)==2)
                {
                    cout<<"***** Welcome User Into the Animal Sound Server :: *****\n\nSuccessfully Logged in!  \n";
                   
                     

                    loginStats=1;
                }
				
				//3
                else
                {
                    char alert[]= "\nUserid or Password incorrect, Please Try Again.. \n\n";
					write(1,alert,strlen(alert));

                    bzero(buff,MAX);
                    loginStats=-1;

					char title[]="\nUserId : ";
					write(1,title,strlen(title));
    
                }
            }
			
 			else
			{           
 				string userIp = buff;

				vector<string>input;

 
				stringstream ss(userIp);
				while(getline(ss,userIp,' '))
				{   
					if(userIp.size()==0) continue;
					input.push_back(userIp);
				}
				
 				userIp=input[0];

 				bzero(buff,MAX);

 				n=read(sockFd , buff , sizeof(buff));
				

 				if(n==0)
				{	
					if(userIp.compare("bye")==0)
					{
						cout<<"\n******* Now Your Connection With the Server is close, Bye!  ***** \n\n";
						close(sockFd);
						exit(0);
					}

					else
					{
						cout<<"**** : Server Terminated Prematurely : ****\n\n";
						close(sockFd);
						exit(0);						
					}
					                
				}

 		
				string result = buff;
				 

 
				if(userIp.compare("sound")==0)
				{   
					cout<<"\nSoundServer : "<<result<<"\n";
				}
				 
				
 
				else if(userIp.compare("store")==0)
				{   
					 

					if(result=="success") cout<<"Animal Sound Successfully Stored !  \n";

					else if(result=="fail") cout<<"Missing Arguments , Try Again \n";
					
					 

				}

				 

 
				else if (userIp.compare("query")==0) 
				{
					 

					if(result=="none") cout<<"I DONT KNOW ANY ANIMAL SOUND :( \nQUERY : OK\n";

					else cout<<"\nList of Animal Sounds : \n\n"<<result<<"\n>> QUERY : OK\n";
					
					 
					
				}

				 

				else
				{   
 
					 

					if(result=="none") 
						cout<<"Sorry! I dont Know '"<<userIp<<"'\n";
					
					else 
						cout<<"SOUND : A '"<<userIp<<"' Say '"<<result<<"'\n";
 				}

				 

 				
			}
			
 			write(1,"Client Message : ",sizeof("Client Message : "));
			
        }      
    }

    close(sockFd);
    return 1;
}