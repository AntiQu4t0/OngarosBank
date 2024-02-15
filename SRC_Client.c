#include <iostream>
#include <WS2tcpip.h>
#include <map>
#include <algorithm>
#include <windows.h>
#include <cstring>
#include <string>

using namespace std;

#define PORT 6969

string ip = "localhost";
string utente = "";
string password = "";
int bilancio = 0;

void scritta(const std::string& line) {
    for (char c : line) {
        cout << c;
        cout.flush();
        Sleep(rand() % 100000);
    }
}

string get_rank(int bilancio) {
    map<int, string> ranks = {
        {0, "paoletto"},
        {1000, "modestamente abbiente"},
        {5000, "sufficientemente abbiente"},
        {10000, "abbiente"},
        {20000, "piuttosto abbiente"},
        {50000, "benestante"},
        {100000, "abbastanza ricco"},
        {200000, "piuttosto ricco"},
        {500000, "ricco"},
        {1000000, "molto ricco"},
        {2000000, "estremamente ricco"},
        {5000000, "opulento"},
        {10000000, "baronetto"},
        {50000000, "magnate"},
        {100000000, "ongaro"}
    };

    for (auto it = ranks.rbegin(); it != ranks.rend(); it++) {
        if (bilancio >= it->first) {
            return it->second;
        }
    }
    return "scarso";
}

map<int, int> conta_banconote(int cifra) {
    int banconote[] = { 500, 200, 100, 50, 20, 10, 5, 2, 1 };
    map<int, int> risultato;

    for (size_t i = 0; i < sizeof(banconote) / sizeof(banconote[0]); i++) {
        int banconota = banconote[i];
        int num_banconote = cifra / banconota;
        if (num_banconote > 0) {
            risultato[banconota] = num_banconote;
        }
        cifra %= banconota;
    }

    return risultato;
}

void ritiro(int bilancio, SOCKET sock) {
    char x[] = "ritiro";
    send(sock, x, strlen(x), 0);
    cout << "Quanti soldi ti servono: ";
    int cifra;
    cin >> cifra;
    cin.ignore(); // Aggiunto per pulire il buffer
    map<int, int> risultato = conta_banconote(cifra);
    if (cifra > bilancio) {
        scritta("i soldi non si generano....");
        return;
    }
    send(sock, (char*)&cifra, sizeof(cifra), 0);
    scritta("ecco a te, ti do: \n");
    for (auto it = risultato.begin(); it != risultato.end(); it++) {
        cout << it->second << " banconote da " << it->first << " Euro\n";
        cout << "premi un tasto per uscire";
        cin.get();
    }
}

void trasferimento(int bilancio, SOCKET sock) {
    string a;
    int pippo;
    cout << "a chi vuoi fare il bonifico: ";
    cin >> a;
    cout << "di quanto: ";
    cin >> pippo;
    if (pippo > bilancio) {
        cout << "non puoi generare soldi...";
        return;
    }
    else {
        char x[] = "miao";
        send(sock, x, strlen(x), 0);
        Sleep(200000);
        string da_mada = a + "," + to_string(pippo);
        send(sock, da_mada.c_str(), da_mada.length(), 0);
    }
}

void pizzo(int bilancio, SOCKET sock) {
    string a;
    int x;
    cout << "A che stato/associazione malavitosa vuoi mandare i soldi: ";
    cin >> a;
    cout << "Di quanto: ";
    cin >> x;
    if (x > bilancio) {
        cout << "non puoi generare soldi...";
        return;
    }
    scritta("Spedisco anonimamente (o forse no) " + to_string(x) + "€ alla " + a + "\n");
    Sleep(2000000);
    scritta("fatto, Grazie per averci scelto");
    char t[] = "pizzo";
    send(sock, t, strlen(t), 0);
    Sleep(300000);
    send(sock, (char*)&x, sizeof(x), 0);
}

void home(string username, int bilancio, string rank, SOCKET sock) {
    scritta(("Buongiorno, " + username + " bentornato").c_str());
    Sleep(3000000);
    cout << R"(
               #################                              INFO:
           #########################                          Usarname: )" << username << endl;
    cout << R"(         #############################                        Bilancio: )" << bilancio << endl;
    cout << R"(       #################################                      Stato: )" << rank << endl;
    cout << R"(     ####################################%
    ###############+:....:=+###############                   AZIONI:
   ###############:.........:*##############                   Ritira il cash: "Q"
  ###############*...........-###############                  Pizzo (ma smart): "E"
 ###############*...........=###############                  Bonifico: "T"
 ###############+...........=################
%################...........*################
%################+.........=#################
%#################=.......=##################
 ##################*-:.:=*###################
 ##############*=:..-=+=-..:=*##############
 #########*-:.................:-*##########
 ##########.....................:*#######%
  #######-.......................-#######
   #####+:.......................:*####%
     ###=.........................+###
         #*=:.....................:+##
           %#*+-:.............:-+*##
               ###*++=====++###


    )" << endl;

    string x;
    cout << "Scegli cosa fare:";
    cin >> x;
    transform(x.begin(), x.end(), x.begin(), ::tolower);
    if (x == "q") {
        ritiro(bilancio, sock);
    }
    if (x == "e") {
        pizzo(bilancio, sock);
    }
    if (x == "t") {
        trasferimento(bilancio, sock);
    }
}

void registrazione(string user, string passw, SOCKET sock) {
    string message = "register[" + user + "," + passw + "]";
    send(sock, message.c_str(), message.length(), 0);
}

void login(string user, string passw, SOCKET sock) {
    system("cls");
    string message = "login[" + user + "," + passw + "]";
    send(sock, message.c_str(), message.length(), 0);
    char response[1024] = { 0 };
    int valread = recv(sock, response, 1024, 0);
    if (valread == -1) {
        cout << "Errore nella ricezione dei dati dal server." << endl;
        return;
    }
    if (valread == 0) {
        cout << "Connessione chiusa dal server." << endl;
        return;
    }
    if (strcmp(response, "1") == 0) {
        cout << "Loggato con successo" << endl;
        Sleep(600000);
        int bilancio;
        int valread = recv(sock, (char*)&bilancio, sizeof(bilancio), 0);
        if (valread == -1) {
            cout << "Errore nella ricezione del bilancio dal server." << endl;
            return;
        }
        string rank = get_rank(bilancio);
        home(utente, bilancio, rank, sock);
    }
    else {
        cout << "password errata" << endl;
        Sleep(2000000);
        closesocket(sock);
        exit(0);
    }
}

void scelta(SOCKET sock) {
    char i;
    cout << R"(
    Scegli un opzione:
    1) Register
    2) login

    metti una scelta: )";
    cin >> i;
    if (i == '1') {
        string user, passw;
        cout << "username: ";
        cin >> user;
        cout << "password: ";
        cin >> passw;
        registrazione(user, passw, sock);
    }
    else if (i == '2') {
        string user, passw;
        cout << "username: ";
        cin >> user;
        cout << "password: ";
        cin >> passw;
        login(user, passw, sock);
    }
    else {
        cout << "metti un opzione valida" << endl;
        Sleep(2000000);
        scelta(sock);
    }
}

int main() {
    cout << R"(
  ____                         _        __             __
 / __ \___  ___ ____ ________ ( )___   / /  ___ ____  / /__
/ /_/ / _ \/ _ `/ _ `/ __/ _ \|/(_-<  / _ \/ _ `/ _ \/  '_/
\____/_//_/\_, /\_,_/_/  \___/ /___/ /_.__/\_,_/_//_/_/\_\
          /___/
by ongaro (credo si capisse)
)";
    Sleep(4000000);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Errore durante l'inizializzazione di Winsock." << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cout << "Errore durante la creazione del socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cout << "Errore durante la connessione al server" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    scelta(sock);

    closesocket(sock);
    WSACleanup();
    return 0;
}
