#define WIN32_LEAN_AND_MEAN // макрос

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;

int main()
{
	// служебная структура для хранение информации (ВЕРСИИ, СТРУКТУРЫ(НАПРИМЕР СЕМЕЙНУЮ)
   // о реализации Windows Sockets
	WSADATA wsaData;//переменная, которая хранит версию, структуры о реализации Windows Sockets
	SOCKET СonnectSocket = INVALID_SOCKET;//прием данных от подключенного клиента
	SOCKET ListenSocket = INVALID_SOCKET;//слушает соединение
	ADDRINFO hints;//хранит доступные сетевые адреса и порты
	ADDRINFO* addrResult;//указатель списка портов и адресов
	const char* sendBuffer = "Hello from Server";//буфер с сообщением 
	char recvBuffer[512];//размер буфера
	char vtoroiBuffer[512];//такой же буфер как и был 

	// старт использования библиотеки сокетов процессом определния версии и структуры
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	int result2 = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Если произошла ошибка подгрузки библиотеки
	if (result != 0) {
		cout << "WSAStartup failed result" << endl;
		return 1;//возвращение кода ошибки
	}

	ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
	hints.ai_family = AF_INET;//4-БАЙТНЫЙ Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// Используем протокол TCP
	hints.ai_flags = AI_PASSIVE;// Пассивная сторона, потому что просто ждет соединения

	// функциия хранит в себе адрес, порт,семейство структур, адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы 
	if (result != 0) {
		cout << "getaddrinfo failed with error" << endl;
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки
	}


	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	// Если создание сокета завершилось с ошибкой, выводим сообщение,
   // освобождаем память, выделенную под структуру addr,
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket error" << endl;
		WSACleanup();//очситка WSAStartup
		freeaddrinfo(addrResult);
		return 1;//возвращение кода ошибки
	}
	// Привязываем сокет к IP-адресу (соединились с сервером)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	// Если привязать адрес к сокету не удалось, то выводим сообщение
   // об ошибке, освобождаем память, выделенную под структуру addr.
   // и закрываем открытый сокет.
   // Выгружаем DLL-библиотеку из памяти и закрываем программу.
	if (result == SOCKET_ERROR) {
		cout << "Binding connect failed <3" << endl;
		closesocket(СonnectSocket);
		ListenSocket = INVALID_SOCKET;
		freeaddrinfo(addrResult);
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки
	}

	//прослушивание взодящих сообщений
	result = listen(ListenSocket, SOMAXCONN);
	//если ошибка в прослушивании, то выводим соообщение об ошибке
	if (result == SOCKET_ERROR) {
		cout << "Listening failed" << endl;
		closesocket(СonnectSocket);//закрытие открытого сокета
		ListenSocket = INVALID_SOCKET;//выгрузка библиотеки DLL
		freeaddrinfo(addrResult);//освобождение памяти, выделенной под структуру addr
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки
	}

	//принятие сообщение от клиента
	СonnectSocket = accept(ListenSocket, NULL, NULL);
	//если сервер не получил сообщение от клиента, то выводим сообщение об ошибке
	if (СonnectSocket == INVALID_SOCKET) {
		cout << "Accepting SOCKET failed" << endl;
		closesocket(СonnectSocket);//закрытие открытого сокета
		ListenSocket = INVALID_SOCKET;//выгрузка библиотеки DLL
		freeaddrinfo(addrResult);//освобождение памяти, выделенной под структуру addr
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки
	}
	closesocket(ListenSocket);//закрываем сокет для входящих сообщений
	//цикл, который принимает сообщение клиента и отправляет сообщение клиенту
	do {
		ZeroMemory(recvBuffer, 512);//зануление памяти
		result = recv(СonnectSocket, recvBuffer, 512, 0);//сообщение от клиента
		result2 = recv(СonnectSocket, vtoroiBuffer, 512, 0);//сообщение от клиента
		if (result > 0) {//условие на отправку сообщения
			//вывод сообщений в консоль
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer << endl;
			cout << "LOL:" << vtoroiBuffer << endl;

			result = send(СonnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
			//если сервер не отправил сообщение
			if (result == SOCKET_ERROR) {
				cout << "failed to send" << endl;
				WSACleanup();//очситка WSAStartup
				return 1;//возвращение кода ошибки
			}
		}
		else if (result == 0)//проверка соединения с клиентом
			cout << "Connection closing" << endl;

		else//проверка на ошибку
			cout << "recv failed with error" << endl;
		closesocket(СonnectSocket);//закрытие открытого сокета
		freeaddrinfo(addrResult);//освобождение памяти, выделенной под структуру addr
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки

	} while (result > 0);
	//завершение передачи данных с клиентом
	result = shutdown(СonnectSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		freeaddrinfo(addrResult);//освобождение памяти, выделенной под структуру addr
		WSACleanup();//очситка WSAStartup
		return 1;//возвращение кода ошибки
	}
	closesocket(СonnectSocket);//закрытие открытого сокета
	freeaddrinfo(addrResult);//освобождение памяти, выделенной под структуру addr
	WSACleanup();//очситка WSAStartup
	return 0;//возвращение кода ошибки




}