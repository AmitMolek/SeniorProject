using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Diagnostics;
using System.Windows.Threading;

namespace client
{

    public class BaseClient
    {
        public const int INSTRUCTION_BUFFER_SIZE = 1024;

        private Socket dataSocket;
        private Socket instructionSocket;

        private String serverAddress;
        private int dataPort;
        private int instructionPort;
        private String userName;

        public BaseClient(String _serverAddress, int _dataPort, int _instructionPort,string _userName)
        {
           
            dataSocket = null;
            instructionSocket = null;

            serverAddress = _serverAddress;
            dataPort = _dataPort;
            instructionPort = _instructionPort;
            userName = _userName;

            GetInstructionSocket();


            GetDataSocket();

            Thread t = new Thread(() => {
                try {
                    connectClient();
                }catch (Exception ex) {
                    Console.WriteLine(ex.Message);
                }

            } );
            t.IsBackground = true;
            t.Start();

            
           //Thread.Sleep(3000);
            
            //sendUserName(userName);
            //ConnectDataSocket();

            // while (true) ;
        }

        public BaseClient() {
        }

        public void connectClient()
        {
            try {
                ConnectInstructionSocket();
            } catch (Exception ex) {
                throw (ex);
            }
        }
        public class FtpException : Exception
        {
            public FtpException(string message) : base(message) { }
            public FtpException(string message, Exception innerException) : base(message, innerException) { }
        }

        public Socket getDataSocket()
        {
            return this.dataSocket;
        }
        void GetInstructionSocket()
        {
            this.instructionSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            if (this.instructionSocket == null)
            {
                Console.WriteLine("cannot create instruction socket");
                return;
            }
        }
        void GetDataSocket()
        {
            this.dataSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            if (this.dataSocket == null)
            {
                Console.WriteLine("cannot create dataSocket ");
                return;
            }
        }

        void ConnectInstructionSocket()
        {
            IPAddress addr = null;
            IPEndPoint ep = null;

            try
            {
                this.instructionSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                addr = IPAddress.Parse(serverAddress);
                ep = new IPEndPoint(addr, this.instructionPort);
                this.instructionSocket.Connect(ep);
            }
            catch (Exception ex)
            {
                // doubtfull
                if (this.instructionSocket != null && this.instructionSocket.Connected) this.instructionSocket.Close();

                throw new FtpException("Couldn't connect to remote server", ex);
            }

            //  Thread ConnectDataSocketConnectionThread = new Thread(Thread_HandleInstruction);
            // ConnectDataSocketConnectionThread.Start();
            Thread_HandleInstruction();
        }
        void ConnectDataSocketThread()
        {
            IPAddress addr = null;
            IPEndPoint ep = null;

            try
            {
                this.dataSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                addr = IPAddress.Parse(serverAddress);
                ep = new IPEndPoint(addr, this.dataPort);
                this.dataSocket.Connect(ep);
            }
            catch (Exception ex)
            {
                // doubtfull
                if (this.dataSocket != null && this.dataSocket.Connected) this.dataSocket.Close();

                throw new FtpException("Couldn't connect to remote server", ex);
            }
        }

        void ConnectDataSocket()
        {
            Thread ConnectDataSocketConnectionThread = new Thread(ConnectDataSocketThread);
        }
        void Thread_HandleInstruction()
        {
            int receivedBytes = -1;
            int instructionBytesCounter = 0;
            
            


            int dataBytesCounter = 0;
            byte[] dataBuffer = new byte[INSTRUCTION_BUFFER_SIZE];


            string instructionSocketFd;
            string dataSocketFd;

            string receivedMsg = ""; /* will be in use to savethe whole msg*/


            while (true)
            {
                instructionBytesCounter = 0;
                while (instructionSocket.Available > 0)
                {
                    byte[] instructionBuffer = new byte[INSTRUCTION_BUFFER_SIZE];
                    receivedBytes = instructionSocket.Receive(instructionBuffer, instructionBuffer.Length, 0);
                    Debug.WriteLine("start reading instruction");
                    instructionBytesCounter += receivedBytes;
                    receivedMsg += instructionBuffer;/* will be in use to savethe whole msg*/

                    string converted = Encoding.UTF8.GetString(instructionBuffer, 0, receivedBytes);
                    Console.WriteLine(" line 162 Msg: {0}", converted);

                    string[] splitMsg = converted.Split('|');

                    foreach (string instruction in splitMsg)
                    {
                        string[] splitInstructions = instruction.Split(':');
                        if (splitInstructions.Length > 0)
                        {
                            if (splitInstructions[0] == "pass")
                            {
                                Console.WriteLine("splitInstructions[0] == \"pass\"");

                                if (splitInstructions[1] == "instruction_socket")
                                {
                                    instructionSocketFd = splitInstructions[2];


                                    IPAddress addr = null;
                                    IPEndPoint ep = null;

                                    addr = IPAddress.Parse(serverAddress);
                                    ep = new IPEndPoint(addr, this.dataPort);
                                    this.dataSocket.Connect(ep);

                                    while (dataSocket.Available <= 0) ;
                                    while (dataSocket.Available > 0)
                                    {
                                        receivedBytes = dataSocket.Receive(dataBuffer, dataBuffer.Length, 0);

                                        Console.WriteLine("start reading data");
                                        dataBytesCounter += receivedBytes;
                                        receivedMsg += dataBuffer;/* will be in use to savethe whole msg*/

                                        converted = Encoding.UTF8.GetString(dataBuffer, 0, dataBytesCounter);
                                        Console.WriteLine(" data Msg: {0}", converted);
                                        string[] splitData = converted.Split(':');


                                        if (splitData[1] == "data_socket")
                                        {
                                            dataSocketFd = splitData[2];

                                            Console.WriteLine("Instruction Socket = {0}", instructionSocketFd);
                                            Console.WriteLine("data Socket = {0}", dataSocketFd);

                                            string dataSocketIdMsg = "|pass:data_socket_id:" + dataSocketFd;
                                            instructionSocket.Send(Encoding.ASCII.GetBytes(dataSocketIdMsg), 0);


                                        }
                                    }

                                    sendUserName(userName);
                                }
                            }
                        }
                    }

                }
            }
            Console.WriteLine("end reading instruction");
        }
        public void sendFile(String fileName, ref DispatcherTimer timer)
        {
            long fileSize;
            string[] splitMsg;
            int bytes = 0;
            int bytesToSend = 0;
            byte[] dataBuffer = new byte[INSTRUCTION_BUFFER_SIZE];

            fileSize = new System.IO.FileInfo(fileName).Length;
            splitMsg = fileName.Split('\\');
            Console.WriteLine((string)("|pass:file_send:" + splitMsg [splitMsg.Length-1] + "," + fileSize));
            instructionSocket.Send(Encoding.ASCII.GetBytes((string)("|pass:file_send:" + splitMsg[splitMsg.Length - 1] + "," + fileSize)),0);

            FileStream input = File.OpenRead(fileName);
            dataSocket.Send(Encoding.ASCII.GetBytes("|pass:file_start"), 0);

            while ((bytes = input.Read(dataBuffer, 0, dataBuffer.Length)) > 0)
            {
                bytesToSend += bytes;
                dataSocket.Send(dataBuffer, bytes, 0);

                bytesToSend = 0; 
            }
            dataSocket.Send(Encoding.ASCII.GetBytes("|pass:file_end"), 0);
            input.Close();
            timer.Stop();
        }

        public void sendGetFiles(List<string> filesNames) {
            string instruction = "|pass:file_get:";
            for (int i = 0; i < filesNames.Count() - 1; i++) {
                instruction += filesNames[i] + ",";
            }
            instruction += filesNames[filesNames.Count() - 1];

            instructionSocket.Send(Encoding.ASCII.GetBytes(instruction));
        }

        public void sendUserName(string userName)
        {
            instructionSocket.Send(Encoding.ASCII.GetBytes((string)("|pass:user_name:" + userName)));
           
        }

        public void sendGetListFiles() {
            string instruction = "|pass:get_list_files";
            instructionSocket.Send(Encoding.ASCII.GetBytes(instruction));
        }
    }

   
}
