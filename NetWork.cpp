#include <SFML/Network.hpp>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <list>
#include "NetWork.hpp"


void OnePortLic(sf::TcpSocket &socket,int port, bool DEBAG)
{
	sf::TcpListener lis;

	if(DEBAG = true)
	{
		sf::IpAddress IP = sf::IpAddress::getLocalAddress();
		std::cout << "Server local IP:" << IP << std::endl;
	}

	lis.listen(port);
	lis.accept(socket);
}


unsigned short SerCon(sf::TcpSocket &socket, std::string IpName, unsigned short START, unsigned short LAST, bool DEBAG)
{
	sf::Time t1 = sf::seconds(0.25);//Ожидание
	sf::IpAddress IP = IpName;
	sf::Packet packet;
	packet.clear();
	bool done = false;
	unsigned short i;
	for(i = START; i < LAST; i++)
	{
		if(socket.connect(IP, i, t1) == 0)
		{
			done = true;
			if(DEBAG)
				std::cout << "Port:" << i << std::endl;
			break;
		}

	}
	if(!done)
		return 0;	
	return i;
}

bool _SendData(sf::TcpSocket &socket, SData* data, int size)
{
	sf::Packet packet;
	packet.clear();
	packet << size;
	for(int i = 0; i < size; i++)
	{
		packet << data[i].Num;
		packet << data[i].New;
		packet << data[i].Name;
		packet << data[i].Com;
	}
	socket.send(packet);
	return true;
}

SData * _RecData(sf::TcpSocket &socket, int &size)//you need char * only
{
	sf::Socket::Status mys;
	size = 0;
	sf::Packet packet;
	packet.clear();
	mys = socket.receive(packet);

	if(mys == sf::Socket::Disconnected || mys == sf::Socket::Error)
	{	/*
		std::cout << "==================================" << std::endl;
		std::cout << "  Client disconnect to port "
				<< socket.getLocalPort() << std::endl;
		std::cout << "==================================" << std::endl;
		*/
		socket.disconnect();
		size = -1;
		return 0;
	}

	packet >> size;
	SData * data = new SData[size];
	for(int i = 0; i < size; i++)
	{
		packet >> data[i].Num;
		packet >> data[i].New;
		packet >> data[i].Name;
		packet >> data[i].Com;
	}
	if(size == 0)
	{
		delete [] data;
		return 0;
	}
	return data;
}

// * * * * * * * SERVER * * * * * * * //

bool Srv::__RecData(sf::TcpSocket & socket)//you need char * only
{
	SData * new_data;
	int PortNum = socket.getLocalPort() - SPort;
	int size = 0;

	socket.setBlocking(false);
	new_data = _RecData(socket,size);
	socket.setBlocking(true);

	if(size == -1)
		return false;

	if(size == 0)
		return true;

	data[PortNum].resize(size);
	for(int i = 0; i < size; i++)
		new_data[i].Num = PortNum + SPort;

	// // // // // // 
	bigmutex.lock();

	for(int i = 0; i < size; i++)
		data[PortNum][i] = new_data[i];
	for(int i = 0; i < updata.size(); i++)
		updata[PortNum][i] = true;

	bigmutex.unlock();
	// // // // // // 
	delete [] new_data;

	if(data[PortNum][0].Com == "!end")
	{
		data[PortNum][0].Com = "\'Go out\'";
		return false;
	}
	std::cout << "==================================" << std::endl;
	std::cout << "  data come from " 
		<< data[PortNum][0].Name;
	std::cout <<  ": " 
		<< data[PortNum][0].Com << std::endl;
	std::cout << "==================================" << std::endl;
	
	return true;
}

void Srv::SendAllData(sf::TcpSocket & socket)
{
	int size = 0;
	int k = 0;

	bigmutex.lock();

	for(int i = 0; i < data.size(); i++)
		if(updata[i][socket.getLocalPort() - SPort])
			for(int j = 0; j < data[i].size(); j++)
				size++;

	SData * DataForSend = new SData [size];
	for(int i = 0; i < data.size(); i++)
	{
		if(updata[i][socket.getLocalPort() - SPort])
		{
			updata[i][socket.getLocalPort() - SPort] = false;
			for(int j = 0; j < data[i].size(); j++)
				if(data[i][j].Num != 0)	
					DataForSend[k++] = data[i][j];	
		}
	}

	bigmutex.unlock();
	_SendData(socket, DataForSend, size);

	delete [] DataForSend;	
}

void Srv::WorkingWithClient(int * ind, bool * end)
{
	int MyPort = *ind;
	delete ind;
	while(1)
	{
		if(!__RecData(sockets[MyPort]))
		{
			sockets[MyPort].disconnect();
			return;
		}

		SendAllData(sockets[MyPort]);
		if(*end)
			return;
		usleep(15);
	}
}

void Srv::BigLins(bool * end)
{
	while(1)
	{
		for(int i = 1; i < LPort - SPort; i++)
		{		
			sf::TcpListener lis;	
			if(sockets[i].getLocalPort() != 0)
				continue;

			if(lis.listen(i + SPort) == sf::Socket::Error)
				continue;

			lis.accept(sockets[i]);

			if(*end)
			{
				for(auto item: ClientThread)
					if(item != 0)
						item -> join();
				return;
			}

			int * ind = new int;
			*ind = i;

			auto _WWC = [&](int * ind){WorkingWithClient(ind, end);};

			ClientThread[i] = new std::thread(_WWC, ind);
				
			std::cout << "==================================" << std::endl;
			std::cout << "  Client connect to port " << i << std::endl;
			std::cout << "==================================" << std::endl;
			break;
		}
	}
}


void Srv::Server()
{
	bool end = false;
	auto _BL = [&] {BigLins(&end);};
	std::thread LThread(_BL);
	std::string com;
	while(1)
	{
		std::cin >> com;
		if(com == "!end")
		{
			end = true;
			sf::TcpSocket socket;
			SerCon(socket, "0.0.0.0",10000,13000);
			LThread.join();
			return;
		}
		usleep(15);
	}
}

Srv::Srv(int SPort, int LPort)
{
	this->LPort = LPort;
	this->SPort = SPort;
	data.resize(LPort - SPort);
	ClientThread.resize(LPort - SPort);
	updata.resize(LPort - SPort);
	for(auto& item: updata)
		item.resize(LPort - SPort);
	sockets = new sf::TcpSocket[LPort - SPort];

}

Srv::~Srv()
{
	delete [] sockets;
	for(auto item: ClientThread)
		delete item;
}

SData::SData(std::string name, std::string com):
	Name(name), Com(com) {}
