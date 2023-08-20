#include <cli.hpp>
#include <string>

int main(int argc, const char *args[])
{
    if (argc >= 3)
    {
        // if we have ip and port arguments
        ChatClient client(args[1], 8080, std::stoi(args[2]));
        client.connect();
        client.show_self_ip();
        exit(0);
    }
    else
    {
        ChatClient client("127.0.0.1", 8080,13320);
        client.connect();
        client.show_self_ip();
        std::string mesinfo;
        DataProcessor temp;
        temp.init();
        temp.addinfo("content", "today_is_very_good!");
        temp.addinfo("receiverIP", "127.0.0.1");
        temp.addinfo("receiverPort", "1030");
        mesinfo = temp.Getstring();
        std::cout << mesinfo << '\n';
        client.sendMessage(mesinfo);
    }
    return 0;
}