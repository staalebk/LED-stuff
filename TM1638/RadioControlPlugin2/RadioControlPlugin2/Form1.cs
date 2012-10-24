using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using Microsoft.FlightSimulator.SimConnect;
using System.Runtime.InteropServices;


namespace RadioControlPlugin2
{
    public partial class Form1 : Form
    {
        

        double com1Freq = 118.225;
        double com1Standby = 118.925;

        double com2Freq = 118.225;
        double com2Standby = 118.925;

        bool showActive = true;
        int showRadio = 1;

        XPlane xplane = null;

        FSX fsx = null;

        public Form1()
        {
            InitializeComponent();

            setButtons(true, false, false);
        }
        // Simconnect client will send a win32 message when there is 
        // a packet to process. ReceiveMessage must be called to
        // trigger the events. This model keeps simconnect processing on the main thread.

        protected override void DefWndProc(ref Message m)
        {
            if (m.Msg == FSX.WM_USER_SIMCONNECT)
            {
                
                if (fsx != null)
                {
                    fsx.ReceiveMessage();
                }
            }
            else
            {
                base.DefWndProc(ref m);
            }
        }

        private void setButtons(bool bConnect, bool bGet, bool bDisconnect)
        {
            buttonConnect.Enabled = bConnect;
            buttonIncKhz.Enabled = bDisconnect;
            buttonDecKhz.Enabled = bDisconnect;
            buttonIncMhz.Enabled = bDisconnect;
            buttonDecMhz.Enabled = bDisconnect;
            buttonDisconnect.Enabled = bDisconnect;
        }

        public void closeConnections()
        {
            if (fsx != null)
            {
                fsx.CloseConnection();
            }

            if (xplane != null)
            {
                xplane.CloseConnection();
            }

            if (serialPort.IsOpen)
            {
                closeSerialPort();
            }
        }

        // The case where the user closes the client
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            closeConnections();
        }

        public void setFrequencies(double com1A, double com1S, double com2A, double com2S)
        {
            this.com1Freq = com1A;
            this.com1Standby = com1S;
            this.com2Freq = com2A;
            this.com2Standby = com2S;

            if (this.com1Freq != com1A || this.com1Standby != com1S || this.com2Freq != com2A || this.com2Standby != com2S)
            {
                updateSerialDisplay();
            }
        }

        private void closeSerialPort()
        {
            serialPort.Close();
        }

        private void initSerialPort()
        {
            if (ddPort.Text.Length > 0)
            {
                serialPort.PortName = ddPort.Text;
                serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);
                serialPort.Open();
            }
        }

        void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            int bytesToRead = serialPort.BytesToRead;

            for (int i = 0; i < bytesToRead; i++)
            {
                char c = (char)serialPort.ReadChar();

                switch (c)
                {
                    case '0':
                        if (showRadio == 1)
                        {
                            showRadio = 2;
                        }
                        else
                        {
                            showRadio = 1;
                        }
                        Console.WriteLine("Now showing radio " + showRadio);
                        updateSerialDisplay();
                        break;

                    case '1':
                        showActive = !showActive;
                        Console.WriteLine("Showing active radio: " + showActive);
                        updateSerialDisplay();
                        break;

                    case '2':
                        Console.WriteLine("Swapping in radio");
                        sendFreqSwap(showRadio);
                        break;

                    case '4':
                        showActive = false;
                        changeFreqMhz(showRadio, -1);
                        break;

                    case '5':
                        showActive = false;
                        changeFreqMhz(showRadio, 1);
                        break;

                    case '6':
                        showActive = false;
                        changeFreqKhz(showRadio, -1);
                        break;

                    case '7':
                        showActive = false;
                        changeFreqKhz(showRadio, 1);
                        break;

                    default:
                        Console.WriteLine("Unknown char: " + c);
                        break;
                }
            }
        }

        public double COM1Active
        {
            get { return this.com1Freq; }
            set { this.com1Freq = value; }
        }

        public double COM1Standby
        {
            get { return this.com1Standby; }
            set { this.com1Standby = value; }
        }

        public double COM2Active
        {
            get { return this.com2Freq; }
            set { this.com2Freq = value; }
        }

        public double COM2Standby
        {
            get { return this.com2Standby; }
            set { this.com2Standby = value; }
        }

        public void updateSerialDisplay()
        {
            // 1 byte - header (0xFF)
            // 1 byte - dots
            // 1 byte - r
            // 1 byte - g
            // 8 byte - text

            byte[] packet = new byte[12];

            packet[0] = 0xFF;
            packet[1] = Convert.ToByte("00001000", 2);
            if (showActive)
            {
                packet[2] = Convert.ToByte("10000000", 2);
                packet[3] = Convert.ToByte("00000000", 2);
            }
            else
            {
                packet[2] = Convert.ToByte("00000000", 2);
                packet[3] = Convert.ToByte("10000000", 2);
            }


            String text = showRadio + " ";

            if (showRadio == 1)
            {
                if (showActive)
                {
                    text += com1Freq.ToString().Replace(",", "").PadRight(6, '0');
                }
                else
                {
                    text += com1Standby.ToString().Replace(",", "").PadRight(6, '0');
                }
            }
            else
            {
                if (showActive)
                {
                    text += com2Freq.ToString().Replace(",", "").PadRight(6, '0');
                }
                else
                {
                    text += com2Standby.ToString().Replace(",", "").PadRight(6, '0');
                }
            }

            byte[] baText = ASCIIEncoding.ASCII.GetBytes(text);

            Array.Copy(baText, 0, packet, 4, baText.Length);

            if (serialPort.IsOpen)
            {
                serialPort.Write(packet, 0, packet.Length);
            }
        }
        
        private void buttonConnect_Click(object sender, EventArgs e)
        {
            if (fsx == null)
            {
                try
                {
                    // the constructor is similar to SimConnect_Open in the native API
                    fsx = new FSX(this);
                    fsx.OpenConnection();
                    setButtons(false, true, true);
                    initSerialPort();
                }
                catch (COMException ex)
                {
                    displayText("Unable to connect to FSX: " + ex.Message);
                }
            }
            else
            {
                displayText("Error - try again");
                closeConnections();

                setButtons(true, false, false);
            }
        }

        private void buttonDisconnect_Click(object sender, EventArgs e)
        {
            closeConnections();
            closeSerialPort();
            setButtons(true, false, false);
        }

        
        private void changeFreqMhz(int radio, int direction)
        {
            if (fsx != null)
            {
                Console.WriteLine("FSX: Changing Mhz of COM" + radio + ": " + direction);
                fsx.changeFreqMhz(radio, direction);
            }

            if (xplane != null)
            {
                Console.WriteLine("XPLANE: Changing Mhz of COM" + radio + ": " + direction);
                xplane.changeFreqMhz(radio, direction);
                updateSerialDisplay();
            }
        }

        private void changeFreqKhz(int radio, int direction)
        {
            if (fsx != null)
            {
                Console.WriteLine("FSX: Changing Khz of COM" + radio + ": " + direction);
                fsx.changeFreqKhz(radio, direction);
            }

            if (xplane != null)
            {
                Console.WriteLine("XPLANE: Changing Khz of COM" + radio + ": " + direction);
                xplane.changeFreqKhz(radio, direction);
                updateSerialDisplay();
            }
        }

        private void sendFreqSwap(int comradio)
        {
            showActive = true;

            if (fsx != null)
            {
                Console.WriteLine("FSX: Swapping frequencies");
                fsx.sendFreqSwap(comradio);
            }

            if (xplane != null)
            {
                Console.WriteLine("XPLANE: Swapping frequencies");
                xplane.sendFreqSwap(comradio);
                updateSerialDisplay();
            }
        }

        // Response number
        int response = 1;

        // Output text - display a maximum of 10 lines
        string output = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

        void displayText(string s)
        {
            // remove first string from output
            output = output.Substring(output.IndexOf("\n") + 1);

            // add the new string
            output += "\n" + response++ + ": " + s;

            // display it
            richResponse.Text = output;
        }

        private void buttonStopRequest_Click(object sender, EventArgs e)
        {
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();

            Console.WriteLine("Found " + ports.Length + " COM ports");

            ddPort.Items.Clear();
            foreach(String p in ports) 
            {
                ddPort.Items.Add(p);
            }

            if (ddPort.Items.Count > 0)
            {
                ddPort.Text = ddPort.Items[0].ToString();
            }
        }

        private void buttonIncMhz_Click(object sender, EventArgs e)
        {
            changeFreqMhz(showRadio, 1);
        }

        private void buttonDecMhz_Click(object sender, EventArgs e)
        {
            changeFreqMhz(showRadio, -1);
        }

        private void buttonIncKhz_Click(object sender, EventArgs e)
        {
            changeFreqKhz(showRadio, 1);
        }

        private void buttonDecKhz_Click(object sender, EventArgs e)
        {
            changeFreqKhz(showRadio, -1);
        }

        private void buttonXPlaneStart_Click(object sender, EventArgs e)
        {
            if (xplane == null)
            {
                try
                {
                    xplane = new XPlane(49000, this);
                    xplane.StartListening();

                    setButtons(false, true, true);
                    initSerialPort();
                }
                catch (COMException ex)
                {
                    displayText("Unable to connect to X-Plane: " + ex.Message);
                }
            }
            else
            {
                displayText("Error - try again");
                closeConnections();

                setButtons(true, false, false);
            }
        }
        
    }
}
