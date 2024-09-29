#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <uchar.h>
#include <time.h>
#include <openssl/evp.h>
#include <mysql.h>


int tunnel;
int fd, err;
  struct sockaddr_in6 addr;
// struct sockaddr_in addr;

int wstatus;
pid_t w;



char packet2[4096];
socklen_t addrlen;


char MSG[1500];	
int outputFile2;    

int n;

int sys;


 

time_t lastGreetingTime;

int sock;


 



int main()
{

	 

	
	char strS2[37];
	 
	 pid_t pID2 = getpid();
	 sprintf(strS2, "/home/hp/Desktop/VPNServerLogs/file%d", (int) pID2); 

	// sprintf(strS2, "/home/hp/Desktop/androidVpnServerLog"); // THIS PART WORKS
	


	// DON'T FORGET TO DELETE THE FILE AFTER USING IT								 
	outputFile2 = open(strS2, O_RDWR | O_CREAT);
	// outputFile2 = open(strS2, O_RDWR | O_TRUNC); // THIS PART WORKS
	sprintf(MSG, "open: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));
	// outputFile2 = open(strS2, O_RDWR | O_CREAT | O_APPEND);
	chmod(strS2, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
	sprintf(MSG, "chmod: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));


	 

	// n = recv(0, packet2, sizeof(packet2), 0);
	n = recvfrom(0, packet2, sizeof(packet2), 0,
	(struct sockaddr *)&addr, &addrlen);  

	sprintf(MSG, "recv SOCK_STREAM: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));
	sprintf(MSG, "recv SOCK_STREAM Bytes: %d\n", n);
	write(outputFile2, MSG, strlen(MSG));

 
	  tunnel = socket(AF_INET6, SOCK_DGRAM, 0);
	 // tunnel = socket(AF_INET, SOCK_DGRAM, 0);
	// printf("socket: %s: %d\n", strerror(errno), errno);
	sprintf(MSG, "socket: %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));

	int flag = 1;
	setsockopt(tunnel, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	// printf("setsockopt: SO_REUSEADDR %s: %d\n", strerror(errno), errno);
	sprintf(MSG, "setsockopt: SO_REUSEADDR %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));

	// ////////////////////////////// EXPERIMENT SEEMS TO BE WORKING ////////////////////////////

	setsockopt(tunnel, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
	// printf("setsockopt: %s: %d\n", strerror(errno), errno);
	sprintf(MSG, "setsockopt: %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));

	// /////////////////////////////////////////////////////////////////////////////////////////

	  flag = 0;
	  setsockopt(tunnel, IPPROTO_IPV6, IPV6_V6ONLY, &flag, sizeof(flag));
	// printf("setsockopt: %s: %d\n", strerror(errno), errno);
	sprintf(MSG,  "setsockopt: %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));  

	memset(&addr, 0, sizeof(addr));
  addr.sin6_family = AF_INET6;
	  // addr.sin_family = AF_INET;
	   addr.sin6_port = htons(2021);
	 // addr.sin_port = htons(2021);
	// addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
	// addr.sin_addr.s_addr = inet_addr("192.168.1.4");
	// addr.sin_addr.s_addr = inet_addr("10.42.0.1");




	/* ***********************EXPERIMENT WORKS SOMETIMES *********************************/
	   bind(tunnel, (struct sockaddr *) &addr, sizeof addr);	
	sprintf(MSG,  "bind: %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));   

	/* ************************************************************************************* */

	sprintf(MSG, "listening \n");
	write(outputFile2, MSG, strlen(MSG));

	if(n > 0)
	{
		 sprintf(MSG, "n > 0YAY YAY YAY YAY YAY YAY YAY YAY YAY\n");
		 write(outputFile2, MSG, strlen(MSG));
	}

	
	

	/***************************************************************************/
	/***************************************************************************/
	/***************************************************************************/
	if(mysql_library_init(0, NULL, NULL))
	{
		sprintf(MSG, "could not initialize MySQL client library\n");
		write(outputFile2, MSG, strlen(MSG)); 
		return 0;
	}

	MYSQL mysql;
	mysql_init(&mysql);

	if (mysql_real_connect(&mysql, "localhost", "root", "", "hman", 0, NULL, 0) == NULL)
	{
		sprintf(MSG, "Failed to connect to database: Error: %s\n", mysql_error(&mysql));
		write(outputFile2, MSG, strlen(MSG)); 
		return 0;
	}

	if(mysql_query(&mysql, "select tunName from connectionInfo WHERE isAssigned = 'NO' LIMIT 1"))
	{
		sprintf(MSG, "FAILED mysql_query: Error: %s\n", mysql_error(&mysql));
		write(outputFile2, MSG, strlen(MSG)); 
	}

	MYSQL_RES *result;
	result = mysql_store_result(&mysql);

	char tunName[10];
	// char *tunName;

	unsigned int num_fields;
	MYSQL_ROW row;

	if(result != NULL)
	{
		num_fields = mysql_num_fields(result);
		sprintf(MSG, "num_fields: %d\n", num_fields);
		write(outputFile2, MSG, strlen(MSG)); 


		while((row = mysql_fetch_row(result)))
		{
			for(int i = 0; i < num_fields; i++)
			{
				// tunName = row[i]; // WATCH OUT FOR CODE LIKE THIS i.e accessing info from a MYSQL_ROW structure
				 sprintf(tunName, "%s", row[i]);
				sprintf(MSG, "tunName: %s \n", tunName);
				write(outputFile2, MSG, strlen(MSG)); 
			}

			 

		}
	}

	mysql_free_result(result);

	char query[300];
	sprintf(query, "UPDATE connectionInfo SET isAssigned = 'YES' WHERE tunName = '%s'", tunName);

	if(mysql_query(&mysql, query))
	{
		sprintf(MSG, "FAILED mysql_query: Error: %s\n", mysql_error(&mysql));
		write(outputFile2, MSG, strlen(MSG)); 
	}

	sprintf(query, "select * from connectionInfo WHERE tunName = '%s'", tunName);
	if(mysql_query(&mysql, query))
	{
		sprintf(MSG, "FAILED mysql_query: Error: %s\n", mysql_error(&mysql));
		write(outputFile2, MSG, strlen(MSG)); 
	}

	result = mysql_store_result(&mysql);
	if(result != NULL)
	{
		
		row = mysql_fetch_row(result);
	
	}

	char *localAddress = row[1];
	char *remoteAddress = row[2];
	char *remoteAddressPrefix = row[3];
	char *route = row[4];
	char *routePrefix = row[5];
	char *mtu = row[6];

	sprintf(MSG, "localAddress: %s \n", localAddress);
	write(outputFile2, MSG, strlen(MSG)); 
	sprintf(MSG, "remoteAddress: %s \n", remoteAddress);
	write(outputFile2, MSG, strlen(MSG)); 
	sprintf(MSG, "remoteAddressPrefix: %s \n", remoteAddressPrefix);
	write(outputFile2, MSG, strlen(MSG)); 
	sprintf(MSG, "route: %s \n", route);
	write(outputFile2, MSG, strlen(MSG)); 
	sprintf(MSG, "routePrefix: %s \n", routePrefix);
	write(outputFile2, MSG, strlen(MSG)); 
	sprintf(MSG, "mtu: %s \n", mtu);
	write(outputFile2, MSG, strlen(MSG)); 


	/***************************************************************************/
	/***************************************************************************/
	/***************************************************************************/


	char vpnConfiguration[80];
	sprintf(vpnConfiguration, "%s&%s&%s&%s&%s&%s&", remoteAddress, remoteAddressPrefix, route, routePrefix, mtu, localAddress);   // LINUX CLIENT REQUIRES ADDRESS OF REMOTE TUNNEL
	// sprintf(vpnConfiguration, "%s&%s&%s&%s&%s&", remoteAddress, remoteAddressPrefix, route, routePrefix, mtu);

 


	memcpy(packet2, vpnConfiguration, strlen(vpnConfiguration));
	send(0, packet2, sizeof(packet2), 0);
	// send(cfd, packet2, sizeof(packet2), 0);
	// printf("send SOCK_STREAM: %s- %d\n", strerror(errno), errno);
	sprintf(MSG, "send SOCK_STREAM: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));




	/* *********** THIS PART WORKS ********************* */
	close(0);
	// printf("close : %s- %d\n", strerror(errno), errno);
	sprintf(MSG, "close : %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG)); 
	/* *********************************************** */



	/***************************************************/
	/***************************************************/
	if( (fd = open("/dev/net/tun", O_RDWR | O_NONBLOCK)) < 0 )
	{
		sprintf(MSG, "open failed\n");
		write(outputFile2, MSG, strlen(MSG));
	}

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	strncpy(ifr.ifr_name, tunName, IFNAMSIZ);

	if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 )
	{
		close(fd);
		// printf("FAILED ioctl\n");
		sprintf(MSG, "FAILED ioctl\n");
		write(outputFile2, MSG, strlen(MSG));
	}

	struct sockaddr_in addr2;
	memset(&addr2, 0, sizeof(addr2));
	addr2.sin_family = AF_INET;
	addr2.sin_addr.s_addr = inet_addr(localAddress);

	memcpy(&ifr.ifr_addr, &addr2, sizeof(struct sockaddr));

	  sock = -1;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	printf("socket: %s %d\n", strerror(errno), errno);

	if( (err = ioctl(sock, SIOCSIFADDR, (void *) &ifr)) < 0 )
	{
		sprintf(MSG, "FAILED ioctl(fd, SIOCSIFADDR...: %s %d\n", strerror(errno), errno);
		write(outputFile2, MSG, strlen(MSG));
		close(fd);
		 
	}

	ifr.ifr_flags = IFF_UP | IFF_NOARP | IFF_MULTICAST | IFF_POINTOPOINT;
	ioctl(sock, SIOCSIFFLAGS, (void *) &ifr);
	sprintf(MSG, "ioctl(sock, SIOCSIFFLAGS... : %s %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));

	memset(&addr2, 0, sizeof(addr2));
	addr2.sin_family = AF_INET;
	addr2.sin_addr.s_addr = inet_addr(remoteAddress);
	memcpy(&ifr.ifr_dstaddr, &addr2, sizeof(struct sockaddr));

	if( (err = ioctl(sock, SIOCSIFDSTADDR, (void *) &ifr)) < 0 )
	{
		sprintf(MSG, "FAILED ioctl(fd, SIOCSIFDSTADDR...: %s %d\n", strerror(errno), errno);
		write(outputFile2, MSG, strlen(MSG));
		close(fd);
		 
	}


	/***************************************************/
	/***************************************************/


	sprintf(MSG, "Here\n");
	write(outputFile2, MSG, strlen(MSG));

	int isConnected = 0;


 /*
 // // // // // // // // // //  DO NOT DELETE THIS CODE // // // // // // // // // // // // // // // 
// THIS PART IS RETURNING AN ERROR YET THE PROGRAM WORKS PERFECTLY
	connect(tunnel, (struct sockaddr *)&addr, addrlen);
	// printf("connect: %s: %d\n", strerror(errno), errno);
	sprintf(MSG, "connect: %s: %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));
	isConnected = 1; 

// THIS PART IS RETURNING AN ERROR YET THE PROGRAM WORKS PERFECTLY
	fcntl(tunnel, F_SETFL, O_NONBLOCK);
	// printf("fcntl: %s- %d\n", strerror(errno), errno);
	sprintf(MSG, "fcntl: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG)); */


	 
	 lastGreetingTime = time(NULL); // If i don't set this, the if statement below will always be true
	 /* struct timespec tp;
	 struct timespec tp2;
	 clock_gettime(CLOCK_REALTIME , &tp); */

	 
	 /***************** EXPERIMENT ***********************/
	 			int fd3 = open("/home/hp/Desktop/vpnServerPlusForking/key.key", O_RDWR);
				int fd4 = open("/home/hp/Desktop/vpnServerPlusForking/iv.iv", O_RDWR);
				unsigned char key[16];
				unsigned char iv[16];
				if(-1 == read(fd3, key, sizeof(key))) 
				{
					sprintf(MSG, "-1 == read(fd3, key {{{{{{{{{}}}}} \n");
					write(outputFile2, MSG, strlen(MSG));
				}
				if(-1 == read(fd4, iv, sizeof(iv))) 
				{
					sprintf(MSG, "-1 == read(fd4, iv {{{{{{{{{}}}}} \n");
					write(outputFile2, MSG, strlen(MSG));
				}
				EVP_CIPHER_CTX *dectx;
				dectx = EVP_CIPHER_CTX_new();
				EVP_CIPHER_CTX *enctx;
				enctx = EVP_CIPHER_CTX_new();
				unsigned char outbuf[4096];				 
				int outlen;
				int outlen2;
				close(fd3);
				close(fd4);

				 

	 /***************************************************/




	while(1)
	{
				/********************** EXPERIMENT ************************************/
				/* sprintf(MSG, "ANOTHER ROUND\n");
				write(outputFile2, MSG, strlen(MSG)); */

				// sprintf(MSG, "time(NULL) - lastGreetingTime %f\n",   difftime(time(NULL) , lastGreetingTime));
				// write(outputFile2, MSG, strlen(MSG));

				/* clock_gettime(CLOCK_REALTIME , &tp2);
				sprintf(MSG, "tp2 - tp %ld\n",   tp2.tv_sec - tp.tv_sec);
				write(outputFile2, MSG, strlen(MSG)); */

				/**********************************************************************/



		    /************** EXPERIMENT WORKS *************************************/
			   // if((tp2.tv_sec - tp.tv_sec) > 180)
			  if((time(NULL) - lastGreetingTime) > 180) // THIS SOMETIMES WORK AND SOMETIMES DOESN'T. WHEN IT DOESN'T, IT DON'T COUNT OVER 90 or some other value below that.
			 	{
					sprintf(MSG, "I THINK ANDROID VPN CLIENT IS DOWN +++++++++++++++++++\n");
					write(outputFile2, MSG, strlen(MSG));

					sprintf(MSG, "--!!!!!!!!!!!!----------SHUTTING DOWN CONNECTION----!!!!!!!!!!!!!---------SHUTTING DOWN CONNECTION----!!!-- \n");
					write(outputFile2, MSG, strlen(MSG));

					int rValue = close(tunnel);
					// printf("close tunnel return value: %d\n", rValue);
					sprintf(MSG, "close tunnel return value: %d\n", rValue);
					write(outputFile2, MSG, strlen(MSG));
					rValue = close(fd);
					// printf("close fd return value: %d\n", rValue);
					sprintf(MSG, "close fd return value: %d\n", rValue);
					write(outputFile2, MSG, strlen(MSG));

					
					

					sprintf(query, "UPDATE connectionInfo SET isAssigned = 'NO' WHERE tunName = '%s'", tunName);
					if(mysql_query(&mysql, query))
					{
						sprintf(MSG, "FAILED mysql_query: Error: %s\n", mysql_error(&mysql));
						write(outputFile2, MSG, strlen(MSG));
					}

					/*
					int er = execl("/opt/lampp/htdocs/php/phpscript.sh", "phpscript.sh", tunName, NULL);
					*/

					EVP_cleanup();
					close(sock);

					/*ftruncate(outputFile2, 0);   // THIS LINE IS CAUSING SOME TUNNELS TO PREMATURELY CLOSE 
					printf("ftruncate: %s %d\n", strerror(errno), errno); */


					close(outputFile2);
					mysql_close(&mysql);
					mysql_library_end();
					return 0;

			 	}

			/***************************************************************/



			/*********************EXPERIMENT *********************************/
			 	 

			if(1 != EVP_DecryptInit_ex(dectx, EVP_aes_128_cbc(), NULL, key, iv))	
			{
					sprintf(MSG, "1 != EVP_CipherInit_ex dectx {{{{{{{{{}}}}}");
					write(outputFile2, MSG, strlen(MSG));
			}
			if(1 != EVP_EncryptInit_ex(enctx, EVP_aes_128_cbc(), NULL, key, iv))	
			{
					sprintf(MSG, "1 != EVP_CipherInit_ex enctx {{{{{{{{{}}}}}");
					write(outputFile2, MSG, strlen(MSG));
			}

			 


				 

				outlen = 0;
				outlen2 = 0;

			/*****************************************************************/

			n = read(fd, packet2, sizeof(packet2));
			if(n > 0)
			{
				/****************** EXPERIMENT *********************************/
						 
									if(0 == EVP_CipherUpdate(enctx, outbuf, &outlen, packet2, n)) 
									{
											sprintf(MSG, "0 == EVP_CipherUpdate");
											write(outputFile2, MSG, strlen(MSG));
									}else{
											sprintf(MSG, "EVP_CipherUpdate enctx outlen :%d\n", outlen);
											write(outputFile2, MSG, strlen(MSG));
									}

									 
									 	if(0 == EVP_CipherFinal_ex(enctx, outbuf + outlen, &outlen2))
										{
												sprintf(MSG, "0 == EVP_CipherFinal_ex");
												write(outputFile2, MSG, strlen(MSG));
										}else{
												sprintf(MSG, "EVP_CipherFinal_ex enctx outlen2 :%d\n", outlen2);
												write(outputFile2, MSG, strlen(MSG));
										}  
 

				/**************************************************************/
				
				sprintf(MSG, "bytes RECVD FROM TUN0:%d\n", n);
				write(outputFile2, MSG, strlen(MSG));
				// n = sendto(tunnel, packet2, n, 0, (struct sockaddr *) &addr, addrlen); THIS PART WORKS. DON'T DELETE.
				/************* EXPERIMENT **********************/
				 
					
					if (outlen2 != 0)
					{
						 n = sendto(tunnel, outbuf, outlen + outlen2, 0, (struct sockaddr *) &addr, addrlen); 
					}else{
						n = sendto(tunnel, outbuf, outlen, 0, (struct sockaddr *) &addr, addrlen); 
					}
				 

				// EVP_CIPHER_CTX_reset(enctx);

				/******************************************************/
				sprintf(MSG, "Bytes sent to UDP : %d\n", n);
				write(outputFile2, MSG, strlen(MSG));
				sprintf(MSG, "\n");
				write(outputFile2, MSG, strlen(MSG));
				write(outputFile2, MSG, strlen(MSG));
			}


			/************ EXPERIMENT ****************/
				outlen = 0;
				outlen2 = 0;
				

			/***************************************/
		
			n = recvfrom(tunnel, packet2, sizeof(packet2), MSG_DONTWAIT, (struct sockaddr *)&addr, &addrlen);	 // THE BELOW LINE CREATED HEADACHES i.e made the vpn really slow and receive bogus greeting messages
			// n = recvfrom(tunnel, packet2, sizeof(packet2), 0, (struct sockaddr *)&addr, &addrlen);	 
			if(n > 0)
			{
				/******************* EXPERIMENT ****************************************/

						if(0 == EVP_DecryptUpdate(dectx, outbuf, &outlen, packet2, n)) 
						{
								sprintf(MSG, "0 == EVP_DecryptUpdate dectx \n");
								write(outputFile2, MSG, strlen(MSG));
						}else{
								sprintf(MSG, "EVP_DecryptUpdate dectx outlen :%d\n", outlen);
								write(outputFile2, MSG, strlen(MSG));
						}
 						
 						if(0 == EVP_DecryptFinal_ex(dectx, outbuf + outlen, &outlen2))
						{
							  
								sprintf(MSG, "0 == EVP_DecryptFinal_ex dectx outlen2 :%d\n", outlen2);
								write(outputFile2, MSG, strlen(MSG));
						}else{
								sprintf(MSG, "EVP_DecryptFinal_ex dectx outlen2 :%d\n", outlen2);
								write(outputFile2, MSG, strlen(MSG));
						}  
						 
						 

				/**********************************************************************/
				
				// if(packet2[0] == 0) THIS PART WORKS.
				if(outbuf[0] == 0)		
				{
						
						// packet2[n] = 0; THIS PART WORKS.
						 /************* EXPERIMENT *****************/
								outbuf[outlen + outlen2] = 0; 

								
						 /******************************************/
						  
						char * disconnectMessage = "DISCONNECT";

						char * greetingMessage = "HEY SERVER. I AM STILL CONNECTED";

						if(strcmp(disconnectMessage, &outbuf[1]) == 0)
						// if(strcmp(disconnectMessage, &packet2[1]) == 0) THIS PART WORKS.
						{
							sprintf(MSG, "packet2[1] == DISCONNECT\n");
							write(outputFile2, MSG, strlen(MSG));

							
							sprintf(MSG, "DISCONNECTING DISCONNECTING DISCONNECTING -------------------------------------------------------- \n");
							write(outputFile2, MSG, strlen(MSG));

							int rValue = close(tunnel);
							// printf("close tunnel return value: %d\n", rValue);
							sprintf(MSG, "close tunnel return value: %d\n", rValue);
							write(outputFile2, MSG, strlen(MSG));
							rValue = close(fd);
							// printf("close fd return value: %d\n", rValue);
							sprintf(MSG, "close fd return value: %d\n", rValue);
							write(outputFile2, MSG, strlen(MSG));

							 

							/*
							int er = execl("/opt/lampp/htdocs/php/phpscript.sh", "phpscript.sh", tunName, NULL);
							*/

							sprintf(query, "UPDATE connectionInfo SET isAssigned = 'NO' WHERE tunName = '%s'", tunName);
							if(mysql_query(&mysql, query))
							{
								sprintf(MSG, "FAILED mysql_query: Error: %s\n", mysql_error(&mysql));
								write(outputFile2, MSG, strlen(MSG));
							}

							EVP_cleanup();
							close(sock);
							ftruncate(outputFile2, 0);
							printf("ftruncate: %s %d\n", strerror(errno), errno);
							close(outputFile2);
							mysql_close(&mysql);
							mysql_library_end();
							return 0;

					}


					if(strcmp(greetingMessage, &outbuf[1]) == 0)
					// if(strcmp(greetingMessage, &packet2[1]) == 0) THIS PART WORKS.
					{
						sprintf(MSG, "THE ANDROID VPN CLIENT IS STILL CONNECTED ++++++++++++\n");
						write(outputFile2, MSG, strlen(MSG));

						
						 lastGreetingTime = time(NULL);
						// clock_gettime(CLOCK_REALTIME , &tp);
						
					}

			}else{				
				sprintf(MSG, "Received %d bytes FROM UDP\n", n);
				write(outputFile2, MSG, strlen(MSG));
				// n = write(fd, packet2, n); THIS PART WORKS. DON'T DELETE
				/*********EXPERIMENT****************************/
					 
							 if (outlen != 0)
							 {
							 		n = write(fd, outbuf, outlen + outlen2);
							 }else{
							 		n = write(fd, outbuf, outlen);
							 }
								
							 
					    // EVP_CIPHER_CTX_reset(dectx);
					
				/************************************/
				 
				sprintf(MSG, "bytes WROTE to TUN0 :%d\n", n);
				write(outputFile2, MSG, strlen(MSG));
				sprintf(MSG, "\n");
				write(outputFile2, MSG, strlen(MSG));
				write(outputFile2, MSG, strlen(MSG));
			}
	    								    		 
		}

	}
					    	

	    		 

	close(tunnel);
	// printf("close tunnel PARENT: %s- %d\n", strerror(errno), errno); 
	sprintf(MSG,  "close tunnel PARENT: %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));

	close(0);
	// printf("close : %s- %d\n", strerror(errno), errno);
	sprintf(MSG,  "close : %s- %d\n", strerror(errno), errno);
	write(outputFile2, MSG, strlen(MSG));
	    		 		

	close(sock);
	close(outputFile2);

	mysql_close(&mysql);
	mysql_library_end();

	return 0;

}

