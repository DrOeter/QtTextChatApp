#include <stdio.h>  
#include <string> 
#include <string.h>  //strlen  
#include <x86_64-linux-gnu/bits/stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <bitset>
#include <iostream>
#include "socketprx.h"
#include <x86_64-linux-gnu/c++/9/bits/stdc++.h>
#include <vector>
#include <fstream>   
#include <sstream>
#include <thread>
//#include <fcntl.h> 
 
#define TRUE   1  
#define FALSE  0  
#define PORT 15000

using namespace std;

FILE *filep ; 
      
char data[50]; 

int opt = TRUE;   
int master_socket , addrlen , new_socket , client_socket[1000], setit=0, nit=0, notarr[1000],group=0, grmem, fdit=0,outit=0;  
int max_clients = 1000 , activity, i , sd, ii,firstset[1000], secset[1000],first,second, groupa[100][1000], cpair=0;  
    
int max_sd, clsarr[1000], it=0, getsock=1, clpair[1000], clit=0, pidarr, pit=0,pon=0,kak=0,kakon=0, cond=0, grit=0, loc, recco = 0, ready = 0; 
pid_t pid;

pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER;

pthread_t thread1, thread2; 
char inbuf[4], sbuf[1028],delim=' '; 
int p[2],p2[2], ipos = -1, grar[1000]; 

      
fd_set readfds,ofds[10];
      
struct sockaddr_in address;
    
const char * fdarr[30];
char buffer[1024];
    
string outsbuf,clients, clarr[1000],killp, token,str ,ss, term, ngroup;

void exit(){
	string exit="kill ";
	exit += to_string(getpid());
	if(pidarr != 0)if(pidarr != getpid())system(exit.c_str());
}

std::string Num (int Number){
     std::ostringstream ss;
     ss << Number;
     return ss.str();
}

std::string space(int i){						
         if(strlen(Num(i).c_str()) == 1) return "   ";
    else if(strlen(Num(i).c_str()) == 2) return "  ";
    else if(strlen(Num(i).c_str()) == 3) return " ";
    else if(strlen(Num(i).c_str()) == 4) return "";
    else return "error";
}

    
void cllarr(fd_set * readfds, int * master_socket){
        FD_ZERO(readfds);   
    	    	        
        FD_SET(*master_socket, readfds);   
        max_sd = *master_socket;   
      
    	exit();
    	      
        for ( i = 0 ; i < 1000 ; i++){
        	sd = client_socket[i]; 
        	//cout<<"sdsd: "<<sd<<"\n";
        	
        	if(sd > 0){ 
            	FD_SET( sd , readfds);
            }
            
           	if(sd > max_sd) max_sd = sd;   
        }   
        
        std::bitset<16> x(*(long*)readfds);
    	std::cout <<"before_select:  "<< x << '\n';  
    	
    	//usleep(1000000);
    
        activity = select( max_sd + 1 , readfds , NULL , NULL , NULL);  
       
       	std::bitset<16> xy(*(long*)readfds);
    	std::cout <<"after_select:   "<< xy << "\n\n";
       	
        if ((activity < 0) && (errno!=EINTR)){   
            printf("select error");   
        }   
       
        if (FD_ISSET(*master_socket, readfds)){  
            if ((new_socket = accept(*master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
           
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n"
             , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));   
           
            for (; ipos < max_clients; ){  
            	ipos++;
                if( client_socket[ipos] == 0 ){   
                    client_socket[ipos] = new_socket;
                    printf("Adding to list of sockets as %d\n" , ipos);   
                    break;   
                }  
            }
            
            clients.clear();
            	
            for (int ic = 0;ic < 1000 ; ic++){
            	if(!clarr[ic].empty()){
            		clients+="Client: ";
            		clients+=clarr[ic];
            		clients+='\n';
            	}	
    		}
            
            TCP_send(&new_socket ,(char*) clients.c_str() , strlen(clients.c_str()) );
                         
            memset(buffer, '\0', 1024);
            TCP_recv (&new_socket, buffer, 1024);
           
            clarr[it]=buffer;
       		it++;
       		
       		
       		for (int i = 0,c;i < 100 ; i++){
       			c=0;
       			
       			if(groupa[i][0] > 0) c=1;
       			
       			if(c == 1){
       				sprintf((char*)ngroup.c_str(), "$NewGroupHeader %s %s", buffer, Num(new_socket).c_str());
       			
       				TCP_send(&groupa[i][0] ,(char*) ngroup.c_str() , strlen(ngroup.c_str()) );
       				ngroup.clear();
       			}	
       		}
       		recco = 1;
        }
}

static void *quit(void* val) {
    while(1){
        memset(inbuf, '\0', 4);
        read(p[0], inbuf, 4);
        
        printf("Finished reading\n"); 
        
        pthread_mutex_lock(&lock1);
		
		close( client_socket[atoi(inbuf)] );
		  
		clarr[atoi(inbuf)].clear();
		
		for (int i = 0; i < 1000; i++){
			//if (notarr[i] == client_socket[atoi(inbuf)])notarr[i] = -1;
			if (clpair[i] == client_socket[atoi(inbuf)])clpair[i] = 0;
			
		}	
		
		for (int i = 0; i < 100; i++){
			for (int ii = 0; ii < 1000; ii++){
				if (groupa[i][ii] == client_socket[atoi(inbuf)])groupa[i][ii] = -1;
			}
		}
		
        client_socket[atoi(inbuf)] = 0;
        
        cout << "sd: "<< inbuf<<"\n";
        
        for(int i = 0; i<(ipos+3); i++)
        	cout << "socket: "<< client_socket[i]<<"\n";
               	
        pthread_mutex_unlock(&lock1);
    }
    return NULL;
}

static void *sendto(void* val) {
	while(1){
		memset(sbuf, '\0', 1028);
    	read(p2[0], sbuf, 1028);
    	//sleep(100000);
    	
    	//int lim = string(sbuf).find_first_of(" ");
    	
    	cout<<sbuf<<"\n\n";
    
		if(-1 == send( atoi(string(sbuf).substr(0,4).c_str()), string(sbuf).substr(4, strlen(sbuf)-4).c_str()  , 1024, 0))cout<<new_socket<<" error42\n\n";
	}	
}	

     
int main(int argc , char *argv[]){ 
	pipe(p2);
    pipe(p); 


  	pthread_mutex_init(&lock1, NULL);
       
    pthread_create( &thread1, NULL, &quit, NULL );
    pthread_create( &thread2, NULL, &sendto, NULL );
    
       
    
    for (i = 0; i < max_clients; i++){   
        client_socket[i] = 0;   
    }   
    
    for(int a=0;a<100;a++){
        for(int b=0;b<1000;b++){
            groupa[a][b]=0;
            grar[b]=0;
        }
    }   
         
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
          
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    
    printf("Listener on port %d \n", PORT);   
         
    if (listen(master_socket, 5) < 0)   {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
    
    pidarr=getpid();
    
    int onn=0;
    
  	
    
    while(TRUE){ 
    	
    	
    	cllarr(&readfds, &master_socket);
    	
        for (i = 0; i < max_clients; i++){   
            sd = client_socket[i];  
            
            if (FD_ISSET( sd , &readfds)){ 
            		memset(buffer, '\0', 1024);
            		
            		if(recco == 1)TCP_recv (&sd, buffer, 13);
            		recco = 0;
            		
            		//printf("header: %s   %ld\n",buffer, strlen(buffer));
            		
            		if(strcmp (buffer, "$TargetHeader") == 0){
            			pid = fork();
         				if(pid == 0){
         					while(1){
         						B:
         						memset(buffer, '\0', 1024);
         						TCP_recv (&new_socket, buffer, 1024);
         						cout << "single: "<< buffer <<"\n";
         				
         						buffer[strlen(buffer)]='\0';
         						
         						if(strncmp (buffer, "$NewTargetHeader",16) == 0){ 
                					int i = 0;
                					for(;grar[i] != 0 ; i++);
                							
                					grar[i] = atoi(string(buffer).substr(17, strlen(buffer)-17).c_str());
                							
                					//cout<<"client: "<<grmem<<"  "<<groupa[grit][i]<<"\n\n";
                						
                					goto B;
                				}
                						
         						if(strncmp (buffer, "$KeepHeader", 11) == 0){
         							size_t start;
									size_t end = 0;
									int it=0;
									
									string str = string(buffer).substr(12,strlen(buffer)-12);
									
         							while ((start = str.find_first_not_of(' ', end)) != string::npos){
										end = str.find(' ', start);
        								token=str.substr(start, end - start);
        					
        								grar[it] =	atoi(token.c_str());
        								it++;
									}
									goto B;
         						}
         						
				 					
				 				
                				if(strcmp (buffer, "$StandbyHeader\0") == 0){
                					exit();
               		 			}	
               		 			
               		 			if(strncmp (buffer, "$ExitHeader",11) == 0){
                							printf("exit %d\n",new_socket);
                							term = buffer;
                							
                							for(int i = 0;i < 1000; i++)
                								if(grar[i] == atoi(term.substr(12, strlen(term.c_str())-12).c_str())){
                								grar[i] = -1;
                								break;
                							}	
                						}
                						
                				if(strcmp (buffer, "$QuitHeader\0") == 0){
                							int at = 0;
                							for(;new_socket != client_socket[at];at++);
                							
                							write(p[1], Num(at).c_str() , 4);
                							
                							term = "$ExitHeader " + Num(new_socket);
                					
                							for(int i = 0; grar[i] != 0; i++){
        										if(grar[i] != -1 && grar[i] != new_socket){
        											if(i < it) TCP_send(&grar[i], (char*)term.c_str() , strlen(term.c_str()) );
        											
        											else write(p2[1], (Num(grar[i]) + space(grar[i])  + term).c_str() , 1028);
        										}
        									}
                							exit();
               		 					}
               		 			
               		 			if(strncmp (buffer, "$StandbyHeader", 14) != 0 && strncmp (buffer, "$ExitHeader", 11) != 0){
               		 						
  											for(int i = 0; grar[i] != 0; i++){
  												cout<<"grar: "<<grar[i]<<"\n";
  											
        										if((new_socket != grar[i]) && (grar[i] != -1)){
        											if(i < it)TCP_send(&grar[i], buffer , 1024);
        											
        											else {
        												//const char *space;
        										
        												//if(strlen(Num(grar[i]).c_str()) == 1) space = "   ";
                										//else if(strlen(Num(grar[i]).c_str()) == 2) space = "  ";
                										//else if(strlen(Num(grar[i]).c_str()) == 3) space = " ";
        										
        												write(p2[1], (Num(grar[i]) + space(groupa[grit][i]) + string(buffer)).c_str() , 1028);
        											}
        										}	
        									}	
        								}
        								
               		 			
         					}	
         				}
         				exit();
           			}
            		
            		buffer[strlen(buffer)]='\0';
            		
            		//printf("rec: %s\n\n",buffer);
            
            		for (int i = 0; i < ipos; i++){
                	
               			int *foo = std::find(std::begin(clpair), std::end(clpair), client_socket[i]);
               			
               			//printf("clsock: %d   %d\n",clpair[i],client_socket[i]);
						
						if (foo != std::end(clpair)){}
						else if((client_socket[i] != 0) && (strcmp (buffer, "$SuperxHeader") == 0)) getsock=0;
               		}	
                
                if(getsock==0){
                	getsock=1;
                	
                	memset(buffer, '\0', 1024);
                	TCP_recv (&sd, buffer, 1024);
                	
                	buffer[strlen(buffer)]='\0';
                	
                	if(strncmp (buffer, "$GroupHeader",12) == 0){
                		str=buffer;
                		size_t start;
						size_t end = 0;
						int yeet=0;
						
                		while ((start = str.find_first_not_of(delim, end)) != string::npos){
							end = str.find(delim, start);
        					token=str.substr(start, end - start);
        					
        					for (loc = 0;strcmp (token.c_str() ,clarr[loc].c_str()) != 0 ; loc++);
        					
        					if((client_socket[loc] != 0) && (loc != 1001))groupa[grit][yeet]=client_socket[loc];
        					yeet++;
						}
						
						groupa[grit][0] = sd;
						
						for(int i = 1;i < yeet; i++){
							TCP_send(&groupa[grit][i], (char*)"$StandbyHeader" , 14);
							usleep(100000);	
						}
                		
                		for(int i = 0; groupa[grit][i] != 0; i++){
                			clpair[clit] = groupa[grit][i];
							clit++;
                		}	
                		
						for(int i = 0; groupa[grit][i] != 0; i++){  
    						if(pid != 0) { 
    							int at = 0;
                				for(;groupa[grit][i] != client_socket[at];at++);
                				
                				clarr[at].clear();
    							
        						pid = fork(); 

        						if (pid == 0) { 
        							grmem = groupa[grit][i];
        							
            						while(1){
            							A:
              							memset(buffer, '\0', 1024);
                						TCP_recv (&grmem, buffer, 1024); 
                
                						printf("GET:  %d    %s    %d\n",kak, buffer, grmem);
                						kak++;
                		
                						buffer[strlen(buffer)]='\0';
                						
                						if(strncmp (buffer, "$NewTargetHeader",16) == 0 && grmem != groupa[grit][0]){ 
                							int i = 0;
                							for(;groupa[grit][i] != 0 ; i++);
                							
                							groupa[grit][i] = atoi(string(buffer).substr(17, strlen(buffer)-17).c_str());
                							
                							//cout<<"client: "<<grmem<<"  "<<groupa[grit][i]<<"\n\n";
                						
                							goto A;
                						}
                						
                						if(strncmp (buffer, "$ExitHeader",11) == 0){
                							printf("exit %d\n",grmem);
                							term = buffer;
                							
                							for(int i = 0;i < 1000; i++)
                								if(groupa[grit][i] == atoi(term.substr(12, strlen(term.c_str())-12).c_str())){
                								groupa[grit][i] = -1;
                								break;
                							}	
                						}
                						
                						if(strcmp (buffer, "$QuitHeader\0") == 0){
                							int at = 0;
                							for(;grmem != client_socket[at];at++);
                							
                							write(p[1], Num(at).c_str() , 4);
                							
                							term = "$ExitHeader " + Num(grmem);
                					
                							for(int i = 0; groupa[grit][i] != 0; i++){
        										if((grmem != groupa[grit][i]) && (groupa[grit][i] != -1)){
        											//const char *space;
        										
        											//if(strlen(Num(groupa[grit][i]).c_str()) == 1) space = "   ";
                									//else if(strlen(Num(groupa[grit][i]).c_str()) == 2) space = "  ";
                									//else if(strlen(Num(groupa[grit][i]).c_str()) == 3) space = " ";
        										
        											if(i >= yeet) write(p2[1], (Num(groupa[grit][i]) + space(groupa[grit][i])  + term).c_str() , 1028);
        											
        											else TCP_send(&groupa[grit][i], (char*)term.c_str() , strlen(term.c_str()) );
        										}
        										
        									}
                							exit();
               		 					}
               		 	
               		 					if(strncmp (buffer, "$StandbyHeader", 14) != 0 && strncmp (buffer, "$ExitHeader", 11) != 0){
               		 						
  											for(int i = 0; groupa[grit][i] != 0; i++){
        										if((grmem != groupa[grit][i]) && (groupa[grit][i] != -1)){
        											//const char *space;
        									
        											//if(strlen(Num(groupa[grit][i]).c_str()) == 1) space = "   ";
                									//else if(strlen(Num(groupa[grit][i]).c_str()) == 2) space = "  ";
                									//else if(strlen(Num(groupa[grit][i]).c_str()) == 3) space = " ";
        										
        											if(i >= yeet) write(p2[1], (Num(groupa[grit][i]) + space(groupa[grit][i]) + string(buffer)).c_str() , 1028);
        											
        											else TCP_send(&groupa[grit][i], buffer , 1024);
        										}	
        									}	
        								}
        								
        								if(strncmp (buffer, "$NewTargetHeader",16) == 0 && i == 0){ 
                							int i = 0;
                							for(;groupa[grit][i] != 0 ; i++);
                							
                							groupa[grit][i] = atoi(string(buffer).substr(17, strlen(buffer)-17).c_str());
                							
                							memset(buffer, '\0', 1024);
                							TCP_recv (&grmem, buffer, 1024);
                							
                							cout<<"client: "<<grmem<<"  "<<buffer<<"\n\n";
                							
                							//const char* space;
                							
                							//if(strlen(Num(groupa[grit][i]).c_str()) == 1) space = "  ";
                							//else if(strlen(Num(groupa[grit][i]).c_str()) == 2) space = " ";
                							//else if(strlen(Num(groupa[grit][i]).c_str()) == 3) space = "";
                							
                							std::ostringstream os;
											for (int i: groupa[grit]) {
												if(i != 0)os << i <<" ";
											}
                							
                							write(p2[1], (Num(groupa[grit][i]) + space(groupa[grit][i]) + "$KeepHeader " + os.str()).c_str() , 1028);
                							sleep(1);
                							write(p2[1], (Num(groupa[grit][i]) + space(groupa[grit][i]) + string(buffer)).c_str() , 1028);
                						}
        							}
        						} 
    						}
						} 
                		exit();
                		grit++;
                	}
            	}
                exit();
            }   
        }   
     }  
    return 0;   
} 

