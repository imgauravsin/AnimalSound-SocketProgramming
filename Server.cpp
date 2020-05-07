#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <utility>

#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<string.h>
#include<unordered_map>
#include<algorithm>
#include<vector>

#define SA struct sockaddr
#define LISTENQ 128
#define MAX 1024
#define CLIENTSIZE 5
using namespace std;

class animalSound
{
    public:        
        
        unordered_map<string,string>sound;
        
        animalSound()
        {
            sound["dog"]="bark";
            sound["cat"]="meow";
            sound["crow"]="kwan";
            sound["snak"]="hiss";
            sound["rat"]="chuchu";
        }

        int getDbSize()
        {
            return sound.size();
        }


        string getSound(string a)
        {   
            if(!sound.count(a)) return "none";
            return sound[a];
        }


        bool isEmpty()
        {
            if(sound.empty()) return true;
            return false;
        }


        bool addAnimalSound(string a,string s)
        {
            if( !sound.count(a) && sound.size()>15) return true;

            sound[a]=s;
            return true;
        }



        string getAnimals()
        {   
            string res="";
            if(sound.empty()) return res;
            int i=1,j=0;
            for(auto it=sound.begin() ; it!=sound.end(); it++)
            {   
                if((j+1)%3==0)
                {
                    res+=std::to_string(i)+". "+it->first+"\n\n";
                }

                else
                {
                    res+=std::to_string(i)+". "+it->first+"  ";
                }

                i++;
                j++;
            }

            return res;
        }
};

 
string lowerCase(string s)
{
    transform(s.begin(),s.end(),s.begin(),::tolower);
    return s;
}

string upperCase(string s)
{
    transform(s.begin(),s.end(),s.begin(),::toupper);
    return s;
}

 
int main()
{   
    
	     
 
    int listenFd, connFd, sockfd, maxfd, maxi, nready, i;
    int Client[FD_SETSIZE],loginStatus[CLIENTSIZE];;
    
    unordered_map<string,string>users;
    unordered_map<int,int>userStatus;

    fd_set allset,rset;
	socklen_t cliLen;
	struct sockaddr_in servaddr, cliaddr;
	
     animalSound* a=new animalSound;
    
	 
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
     
	cout<<"\nSocket successfully created : \n";
	
	 
	bzero(&servaddr, sizeof(servaddr));
	 

	servaddr.sin_port = htons(8080);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	bind(listenFd, (SA*) &servaddr, sizeof(servaddr));

    //char ser[16];
	//cout<<"\nBinding at IP : "<<inet_ntop(AF_INET, &servaddr.sin_addr,ser,sizeof(ser))<<"\n";
    

	 
	
	listen(listenFd, LISTENQ);
	cout<<"\nServer listening State :D \n";

 

	 

    maxfd=listenFd;
    maxi=-1;
    for(i=0; i < CLIENTSIZE; i++)
    {
        Client[i]=-1;
    }

    FD_ZERO(&allset);
    FD_SET(listenFd,&allset);
	 

     for( ; ;)
   {    
       rset=allset;

       
       nready=select(maxfd+1, &rset, NULL, NULL, NULL);
 
     

       if(FD_ISSET(listenFd,&rset))
       {
            cliLen=sizeof(cliaddr);
            connFd =accept(listenFd,(SA*)&cliaddr,&cliLen);
            
            if(connFd < 0)
            {
                cout<<"Client is not Connected .. \n";
                continue;
            }
            
            char ipClient[16];
             

         
            const char* ptr=inet_ntop(AF_INET,&cliaddr.sin_addr,ipClient,sizeof(ipClient));
            cout<<"\nConnected to CLIENT -> ( "<<ptr<<ntohs(cliaddr.sin_port)<<" )\n";

             
             

            for(i=0; i<CLIENTSIZE; i++)
            {    
               
                if(Client[i]<0)
                {
                    Client[i]=connFd;
                    userStatus[connFd]=-1;
                    break;
                }
            }

            if(i==CLIENTSIZE)
            {
                cout<<"ERROR : Server has Too Many Clients! "<<endl;
                close(connFd);
                continue;
            }

          
            FD_SET(connFd,&allset);

          
            if(connFd > maxfd) maxfd=connFd; 

             

             
            if(i > maxi) maxi=i;

            if(--nready <= 0)
            {   
             
                continue;
            }
        }

         

     

        for(i=0; i<=maxi; i++)
        {   

            if( (sockfd = Client[i]) < 0 ) continue;

            if(FD_ISSET(sockfd,&rset))
            {   
                char buff[MAX];
                bzero(buff,MAX);
                
                int n=read(sockfd,buff,sizeof(buff));

                if(n==0)
                {       
                     
                     
                    cout<<"\nClient "<<i+1<<" has not entered any input"<<endl;
                    cout<<"\nClosing Connection with Client "<<i+1<<" :: \n";
                    
                    

                    close(sockfd);
                    FD_CLR(sockfd,&allset);
                    Client[i]=-1;
                    userStatus[sockfd]=-1;
                }

                else
                {   
                     
                    if(userStatus[sockfd]<0)
                    {
                        stringstream userDetails(buff);
                        string id,pass;
                        getline(userDetails,id,' ');
                        getline(userDetails,pass,' ');
                        
                        id=lowerCase(id);
                        pass=lowerCase(pass);

                        int status=-1; 

                         
                        cout<<"\nCredentials Entered by Client : "<<i+1<<endl;
                        cout<<"\nClient : "<<i+1<<" UserID => "<<id<<endl;
                        cout<<"Client : "<<i+1<<" Password => "<<pass<<endl;
                         

                        
                        if(!users.count(id))
                        {   
                            cout<<"\nRegistering client : "<<i+1<<endl;
                            users[id]=pass;
                            userStatus[sockfd]=1;
                            status=htonl(1); 
                            write(sockfd,&status,sizeof(status));
                             
                        }

                        else if(users[id]==pass)
                        {   
                            cout<<"Client :"<<i+1<<" Logged into the server!"<<endl;
                            userStatus[sockfd]=1;
                            status=htonl(2); 
                            write(sockfd,&status,sizeof(status));
                             
                        }

                        else
                        {   
                            cout<<"Credentials Incorrect by => Client : "<<i+1<<endl;
                            userStatus[sockfd]=-1;
                            status=htonl(3); 
                            write(sockfd,&status,sizeof(status));
                             
                        }

                    }

                    
                    else
                    {
                        cout<<"\nClient "<<i+1<<" : "<<buff<<"\n";
                        
                        
                        string userIp=lowerCase(buff);
                        
                        stringstream ss(userIp);
                        vector<string>input;
                        
                        while(getline(ss,userIp,' '))
                        {   
                            if(userIp.size()==0) continue;
                            input.push_back(userIp);
                        }

                        userIp=input[0];

                        bzero(buff,MAX); 

                        
                            
                       
                        
                        if(userIp.compare("sound")==0)
                        {
                            cout<<"\nUSER : Hello Server Are You there : \n";
                            string ok="Yes Animal Sound Server is Active :-))\n";
                            cout<<ok<<endl;

                            std::copy(ok.begin(), ok.end(),buff);
                            write(sockfd,buff,sizeof(buff));
                        }

                         

                    
                        
                        else if(userIp.compare("store")==0)
                        {   
                            
                            if(input.size()!=3)
                            {   
                                write(1,"Failed to store!\n",sizeof("Failed to store!\n"));
                                write(sockfd,"fail",sizeof("fail"));
                            }

                            else
                            {
                                string animal,sound;

                                animal=lowerCase(input[1]);
                                sound=lowerCase(input[2]);

                                if(a->addAnimalSound(animal,sound))
                                {  
                                    string res="success";
                                    std::copy(res.begin(),res.end(),buff);
                                    write(sockfd,buff,sizeof(buff));
                                }
                            }
                        }

                         

                     

                        else if (userIp.compare("query")==0) 
                        {
                            string result= a->getAnimals();
                            if(result=="") result="none";
                            cout<<"\n"<<result<<"\n\n";
                            std::copy(result.begin(), result.end(),buff);
                            write(sockfd,buff,sizeof(buff));
                        }

                          
                        else if (userIp.compare("bye")==0)
                        {   
                            shutdown(sockfd,SHUT_RDWR);
                            close(sockfd);
                            FD_CLR(sockfd,&allset);
                            Client[i]=-1;
                            userStatus.erase(sockfd);
                            cout<<"\nClient "<<i+1<<" says BYE! CLosing Connection..\n";
                         }

 
 
                        
                        else if (userIp.compare("end")==0)
                        {    
                            for(int j=0 ; j<=maxi; j++)
                            {
                                if(Client[j]<0) continue;
                               
                                int tempFd=Client[j];
                              
                                cout<<"Closing client :"<<j+1<<endl;
                                shutdown(tempFd,SHUT_RDWR);
                                close(tempFd);
                                FD_CLR(tempFd,&allset);
                                Client[j]=-1;
                                userStatus.erase(sockfd);
                             
                            }
                            
                             shutdown(listenFd,SHUT_RDWR);
                            FD_ZERO(&allset);
                            close(listenFd);                          
                            
                            cout<<"\nANIMAL SOUND SERVER CLOSED BY CLIENT : "<<i+1<<"\n";
                             exit(0);
                        }

                          
                        
                        else
                        {   
                            cout<<"\nSOUND : "<<a->getSound(userIp)<<endl;
                            string result= a->getSound(userIp);
                            std::copy(result.begin(),result.end(),buff);
                            write(sockfd,buff,sizeof(buff));
                        }

                         
                            
                    }
                    
                                     }

                if(--nready<=0)
                { 
                    break;
                }
            }
        }
    }

   close(listenFd);
   return 1;
}