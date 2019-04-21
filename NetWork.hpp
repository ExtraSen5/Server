#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network.hpp>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <list>


struct SData
{
	bool itis = true;
	bool New = true;
	int Num = 0;
	std::string Name;
	std::string Com;// use 0 to show that 'Com' clear;
	SData(std::string name = "None", std::string com = "None");
};


void OnePortLic(sf::TcpSocket &socket, int port, bool DEBAG = false);
unsigned short SerCon(sf::TcpSocket &socket, std::string IpName, unsigned short START = 10, unsigned short LAST = 3000, bool DEBAG = false);
bool _SendData(sf::TcpSocket &socket,SData* data, int size = 1);
SData * _RecData(sf::TcpSocket &socket, int &size);

class Srv 
{		
	std::vector<std::vector<bool>> updata;
	std::vector<std::vector<SData>> data;
	std::vector<std::thread*> ClientTread;
	std::mutex bigmutex;
	sf::TcpSocket * sockets;
	int SPort;
        int LPort;
	
	Srv(int,int);
	void BigLins();
	void WorkingWithClient(int * ind);
	void SendAllData(sf::TcpSocket & socket);
	bool __RecData(sf::TcpSocket & socket);//you need char * only
public:
	static Srv & get(int SPort, int FPort)
	{
		static Srv __MyServer(SPort,FPort);
		return __MyServer;
	}	
	void Server();
	~Srv();
};

#endif

