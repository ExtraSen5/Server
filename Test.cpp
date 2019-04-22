#include <SFML/Network.hpp>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <functional>
#include "NetWork.hpp"


void foo1(sf::TcpSocket * socket, int MyPort, bool* end)
{
	while(1)
	{
		SData *D = 0;
		int itsize;
		if((D = _RecData(*socket,itsize)) == 0)
		{

			if(socket -> getLocalPort() == 0)
			{
				*end = true;
				exit(0);
			}
			continue;
		}
		for(int i = 0; i < itsize; i++)
			if(D[i].Num != MyPort && D[i].New)
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
	std::string text, name;
	sf::Packet packet;
	sf::IpAddress IP = sf::IpAddress::getLocalAddress(); // ServIPш
	char con = 0;

	std::cout << "Enter (s) for server, Enter (c) for client " << std::endl;
	std::cin >> con;

	if(con == 's')
	{
		std::cout << IP << std::endl;
		Srv::get(10000,13000).Server();
	}	
	else if(con == 'c')
	{	
		std::cout << "Enter your name:";
		std::cin >> name;

		std::cout << "Enter IP:";
		std::cin >> IP_name;
		MyPort = SerCon(socket, IP_name, 10000, 13000);
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
	std::thread threadS(std::bind(&foo1, &socket, MyPort, &end));

	while(1)
	{
		SData S(name);
		std::getline(std::cin,S.Com);
		if(!_SendData(socket, &S, 1) || S.Com == "!end")
			end = true;
		if(end)
			break;
	}

	threadS.join();
	return 0;

}
