using System;
using System.Net;
using System.Net.Sockets;
using System.Windows.Forms;

namespace Client
{
    class Program
    {
        const int port = 10000;

        enum Commands
        {
            test,
            nextSlide,
            prevSlide,
            exit
    }
        const int nextSlide = 1;
        const int prevSlide = 2;
        const int exit = 3;

        static void Main(string[] args)
        {
            Console.WriteLine("Enter this ip into your smartphone app: " + Dns.GetHostAddresses(Dns.GetHostName())[2].ToString());
            Socket listner = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                listner.Bind(new IPEndPoint(IPAddress.Any, port));
                listner.Listen(1);
                Console.WriteLine("\nAwaiting your command\n");
                while (true)
                {
                    Socket socket = listner.Accept();
                    byte[] msg = new byte[1024];
                    int nByte = socket.Receive(msg);
                    if (msg[0] == (int)Commands.nextSlide)
                    {
                        SendKeys.SendWait("{RIGHT}");
                        Console.WriteLine("Switch on next slide");
                    }
                    else if (msg[0] == (int)Commands.prevSlide)
                    {
                        SendKeys.SendWait("{LEFT}");
                        Console.WriteLine("Switch on previous slide");
                    }
                    socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                    if (msg[0] == (int)Commands.exit)
                    {
                        listner.Close();
                        break;
                    }
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine("Error: " + ex.Message);
            }
        }
    }
}
