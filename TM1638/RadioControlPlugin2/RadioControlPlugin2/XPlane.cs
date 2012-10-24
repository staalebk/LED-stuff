using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace RadioControlPlugin2
{
    class XPlane
    {
        private UdpClient udp = null;
        private ClientInfo clientInfo = null;
        private Form1 form1 = null;
        private IPEndPoint xplane_recv = null;
        private bool debug = false;

        public XPlane(int port, Form1 main)
        {
            this.form1 = main;
            IPEndPoint e = new IPEndPoint(IPAddress.Any, port + 3);
            udp = new UdpClient(e);

            xplane_recv = new IPEndPoint(IPAddress.Parse("127.0.0.1"), port);

            Console.WriteLine("Initing with port base at " + port);

            clientInfo = new ClientInfo(udp, e);
        }

        public void StartListening()
        {
            Console.WriteLine("Listening to " + clientInfo.ipEndpoint.Port);
            udp.BeginReceive(new AsyncCallback(this.ReceiveCallback), clientInfo);
        }

        public void CloseConnection()
        {
            udp.Close();
            udp = null;
        }

        public void ReceiveCallback(IAsyncResult ar)
        {
            UdpClient u = (UdpClient)((ClientInfo)(ar.AsyncState)).client;
            IPEndPoint e = (IPEndPoint)((ClientInfo)(ar.AsyncState)).ipEndpoint;

            IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);

            byte[] receiveBytes = u.EndReceive(ar, ref sender);

            String header = ASCIIEncoding.ASCII.GetString(receiveBytes, 0, 4);

            if (header.Equals("DATA"))
            {
                int dataIndex = (int)receiveBytes[5];

                if (dataIndex == 96)
                {
                    float com1freq = fillLastDigit(BitConverter.ToSingle(receiveBytes, 9));
                    float com1stndby = fillLastDigit(BitConverter.ToSingle(receiveBytes, 13));

                    float com2freq = fillLastDigit(BitConverter.ToSingle(receiveBytes, 17));
                    float com2stndby = fillLastDigit(BitConverter.ToSingle(receiveBytes, 21));

                    if (debug)
                    {
                        Console.WriteLine("COM1 Freq: " + com1freq + " <-> " + com1stndby + " | COM2 Freq: " + com2freq + " <-> " + com2stndby);
                    }

                    this.form1.setFrequencies(com1freq, com1stndby, com2freq, com2stndby);
                }
                else
                {
                    Console.WriteLine("Got unknown packet: " + dataIndex);
                }
            }

            u.BeginReceive(new AsyncCallback(ReceiveCallback), clientInfo);
        }

        private float fillLastDigit(float fromXplane)
        {
            if (fromXplane.ToString().EndsWith("2") || fromXplane.ToString().EndsWith("7"))
            {
                return (fromXplane * 10) + 5;
            } else {
                return (fromXplane * 10);
            }
        }

        public void changeFreqMhz(int radio, int direction)
        {
            if (radio == 1)
            {
                if (direction > 0)
                {
                    form1.COM1Standby += 1000;
                }
                else
                {
                    form1.COM1Standby -= 1000;
                }
            }
            else
            {
                if (direction > 0)
                {
                    form1.COM2Standby += 1000;
                }
                else
                {
                    form1.COM2Standby -= 1000;
                }
            }

            sendUpdatedFrequencies();
        }
        /*
        public void changeFreqMhz(int radio, int direction)
        {
            byte[] data = new byte[509];

            Array.Copy(ASCIIEncoding.ASCII.GetBytes("DREF"), data, 4);
            data[4] = 0;
            Array.Copy(BitConverter.GetBytes(-999f), 0, data, 5, 4);

            String path = "";

            if (radio == 1)
            {
                if (direction > 0)
                {
                    path = "sim/radios/stby_com1_coarse_up";
                }
                else
                {
                    path = "sim/radios/stby_com1_coarse_down";
                }
            }
            else
            {
                if (direction > 0)
                {
                    path = "sim/radios/stby_com2_coarse_up";
                }
                else
                {
                    path = "sim/radios/stby_com2_coarse_down";
                }
            }

            ASCIIEncoding.ASCII.GetBytes(path, 0, path.Length, data, 9);

            Console.WriteLine("Sending " + path + " to XPlane");

            udp.Send(data, data.Length, this.xplane_recv);
        }
        */
        public void changeFreqKhz(int radio, int direction)
        {
            if (radio == 1)
            {
                if (direction > 0)
                {
                    form1.COM1Standby += 25;
                }
                else
                {
                    form1.COM1Standby -= 25;
                }
            }
            else
            {
                if (direction > 0)
                {
                    form1.COM2Standby += 25;
                }
                else
                {
                    form1.COM2Standby -= 25;
                }
            }

            sendUpdatedFrequencies();
        }

        public void sendFreqSwap(int comradio)
        {
            if (comradio == 1)
            {
                double t = form1.COM1Active;
                form1.COM1Active = form1.COM1Standby;
                form1.COM1Standby = t;
            }
            else
            {
                double t = form1.COM2Active;
                form1.COM2Active = form1.COM2Standby;
                form1.COM2Standby = t;
            }

            sendUpdatedFrequencies();
        }

        public void normalizeFrequencies()
        {
            form1.COM1Active = keepWithinFreqRange(form1.COM1Active);
            form1.COM1Standby = keepWithinFreqRange(form1.COM1Standby);

            form1.COM2Active = keepWithinFreqRange(form1.COM2Active);
            form1.COM2Standby = keepWithinFreqRange(form1.COM2Standby);
        }

        public double keepWithinFreqRange(double f)
        {
            if (f > 136000) return 118000 + (f - 136000);
            if (f < 118000) return 136000 - (118000 - f);

            return f;
        }

        public void sendUpdatedFrequencies()
        {
            normalizeFrequencies();

            byte[] data = new byte[41];

            // Place header
            ASCIIEncoding.ASCII.GetBytes("DATA", 0, 4, data, 0);

            data[4] = 0;
            data[5] = 96;   // COM radio index
            data[6] = 0;
            data[7] = 0;
            data[8] = 0;

            Array.Copy(BitConverter.GetBytes((float)(form1.COM1Active / 10)), 0, data, 9, 4);
            Array.Copy(BitConverter.GetBytes((float)(form1.COM1Standby / 10)), 0, data, 13, 4);
            Array.Copy(BitConverter.GetBytes((float)(form1.COM2Active / 10)), 0, data, 17, 4);
            Array.Copy(BitConverter.GetBytes((float)(form1.COM1Standby / 10)), 0, data, 21, 4);

            Array.Copy(BitConverter.GetBytes(-999f), 0, data, 25, 4);
            Array.Copy(BitConverter.GetBytes(-999f), 0, data, 30, 4);
            Array.Copy(BitConverter.GetBytes(-999f), 0, data, 34, 4);
            Array.Copy(BitConverter.GetBytes(-999f), 0, data, 37, 4);

            udp.Send(data, data.Length, xplane_recv);
        }
    }
}
