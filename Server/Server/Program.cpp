#include "stdafx.h"


using namespace std;


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