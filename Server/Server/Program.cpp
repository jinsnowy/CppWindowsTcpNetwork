#include "stdafx.h"

#include <NetworkEngine.h>
#include <TcpAcceptorSocket.h>
#include <TcpListener.h>
#include <PacketHandler.h>
#include <Logger.h>
#include <PathManager.h>
#include <AcceptorFactory.h>
#include <GeneratedPacketFactory.h>

using namespace std;
using namespace pkt;
using namespace ns_logger;


int main(int argc, char ** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (argc < 2)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <port>" << endl;;
        return 0;
    }


    return 0;
}