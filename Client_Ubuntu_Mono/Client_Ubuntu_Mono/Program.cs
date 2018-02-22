using System;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;

namespace Client_Ubuntu_Mono
{
	class MainClass
	{
		const int port = 10000;

		enum Command { test, nextSlide, prevSlide, exit };

		public static void Main (string[] args)
		{
			Console.WriteLine ("Enter this ip into your smartphone app: " + Dns.GetHostAddresses(Dns.GetHostName())[0].ToString());
			ProcessStartInfo proc = new ProcessStartInfo();
			Socket listner = new Socket (AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			try
			{
				listner.Bind(new IPEndPoint(IPAddress.Any, port));
				listner.Listen(1);
				Console.WriteLine("\nAwaiting your command\n");
				while(true)
				{
					Socket socket = listner.Accept();
					byte[] msg = new byte[1024];
					int nByte = socket.Receive(msg);
					if (msg[0] == (int)Command.nextSlide)
					{
						proc.FileName = "Right.sh";
						Process.Start(proc);
						Console.WriteLine("Switch on next slide");
					}
					else if (msg[0] == (int)Command.prevSlide)
					{
						proc.FileName = "Left.sh";
						Process.Start(proc);
						Console.WriteLine("Switch on previous slide");
					}
					socket.Shutdown(SocketShutdown.Both);
					socket.Close();
					if (msg[0] == (int)Command.exit)
					{
						listner.Close();
						break;
					}
				}
			}
			catch (Exception ex) {
				Console.WriteLine ("Error: " + ex.Message);
			}
		}
	}
}
