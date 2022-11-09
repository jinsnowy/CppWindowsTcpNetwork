#include "stdafx.h"

#include <NetworkEngine.h>
#include <TcpConnector.h>
#include <PacketHandler.h>
#include <TcpConnectorSocket.h>
#include <Logger.h>
#include <DefaultPacketType.h>
#include <ConnectorFactory.h>
#include <GeneratedPacketFactory.h>

using namespace std;
using namespace ns_logger;

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "arguments invalid." << endl;
        cout << "usage: program <ip> <port>" << endl;;
        return 0;
    }

    return 0;
}