#include <SFML/Network.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <functional>
#include "NetWork.hpp"


void foo1(sf::TcpSocket * socket, int MyPort, bool& end)
{
	while(1)
	{
		SData *D = 0;
		int itsize;
		if((D = _RecData(*socket,itsize)) == 0)
		{

			if(socket -> getLocalPort() == 0)
			{
				end = true;
				exit(0);
			}
			continue;
		}
		for(int i = 0; i < itsize; i++)
			if(D[i].Num != MyPort && D[i].New /*&& D[i].Num != 0*/)
				std::cout << D[i].Name << ": "
			       	       << D[i].Com << std::endl;
		delete [] D;
		usleep(15);
	}

}

int main()
{	
	std::string IP_name;
	unsigned short MyPort = 0;
	sf::TcpSocket socket;
	char buffer[2000];
	std::string text, name;
	char con = 0, mode = 0 ;
	std::cout << "Enter (s) for server, Enter (c) for client " << std::endl;
	std::cin >> con;	
	sf::Packet packet;
	sf::IpAddress IP = sf::IpAddress::getLocalAddress(); // ServIP
	if(con == 's')
	{
		std::cout << IP << std::endl;
		Srv::get(10,3000).Server();
	}	
	else if(con == 'c')
	{	
		std::cout << "Enter your name:";
		std::cin >> name;

		std::cout << "Enter IP:";
		std::cin >> IP_name;
		MyPort = SerCon(socket, IP_name);
		SData S;
		S.Name = name;
		S.Com = "I'm hear.";
		_SendData(socket, &S, 1); 
		std::cout << "==================================" << std::endl;
		std::cout << "  You connect to port " << MyPort << std::endl;
		std::cout << "==================================" << std::endl;
	}
	std::getline(std::cin,text);
	bool end = false;
	std::thread threadS(std::bind(&foo1, &socket, MyPort, end));
	while(1)
	{
		SData S(name);
		std::getline(std::cin,S.Com);
		if(!_SendData(socket, &S, 1) || S.Com == "!end")
			end = true;
		if(end)
			break;
	}
	threadS.detach();
	return 0;

}
