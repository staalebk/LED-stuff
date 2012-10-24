using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.FlightSimulator.SimConnect;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace RadioControlPlugin2
{
    class FSX
    {
        // User-defined win32 event
        public const int WM_USER_SIMCONNECT = 0x0402;

        // SimConnect object
        SimConnect simconnect = null;

        private bool hasRequested = false;

        enum GROUPS
        {
            GROUP1
        }

        enum DEFINITIONS
        {
            Struct1,
        }

        enum DATA_REQUESTS
        {
            REQUEST_1,
        };

        enum EVENT_IDS
        {
            COM_RADIO_WHOLE_DEC,
            COM_RADIO_WHOLE_INC,
            COM_RADIO_FRACT_DEC,
            COM_RADIO_FRACT_INC,
            COM_STBY_RADIO_SWAP,
            COM2_RADIO_WHOLE_INC,
            COM2_RADIO_WHOLE_DEC,
            COM2_RADIO_FRACT_INC,
            COM2_RADIO_FRACT_DEC,
            COM2_RADIO_SWAP
        };

        // this is how you declare a data structure so that
        // simconnect knows how to fill it/read it.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        struct Struct1
        {
            // this is how you declare a fixed size string
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public String title;
            public double com1freq;
            public double com1standby;
            public double com2freq;
            public double com2standby;
        };

        Form1 form1 = null;

        public FSX(Form1 main)
        {
            form1 = main;
        }

        public void ReceiveMessage()
        {
            if(simconnect != null) {
                simconnect.ReceiveMessage();
            }
        }

        public void CloseConnection()
        {

            if (simconnect != null)
            {
                if (this.hasRequested)
                {
                    simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.NEVER, 0, 0, 0, 0);
                }

                // Dispose serves the same purpose as SimConnect_Close()
                simconnect.Dispose();
                simconnect = null;
                Console.WriteLine("Connection closed");
            }
        }

        public void OpenConnection()
        {
            if (simconnect == null)
            {
                try
                {
                    // the constructor is similar to SimConnect_Open in the native API
                    simconnect = new SimConnect("Managed Data Request", form1.Handle, WM_USER_SIMCONNECT, null, 0);
                    initDataRequest();
                }
                catch (COMException ex)
                {
                    Console.WriteLine("Unable to connect to FSX: " + ex.Message);
                }
            }
            else
            {
                Console.WriteLine("Error - try again");
                CloseConnection();
            }
        }

        void simconnect_OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            Console.WriteLine("Connected to FSX");
        }

        // The case where the user closes FSX
        void simconnect_OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            Console.WriteLine("FSX has exited");
            CloseConnection();
        }

        void simconnect_OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            Console.WriteLine("Exception received: " + data.dwException);
        }

        private void startRequest()
        {
            this.hasRequested = true;

            // The following call returns identical information to:
            simconnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.VISUAL_FRAME, SIMCONNECT_DATA_REQUEST_FLAG.CHANGED, 0, 0, 0);

            //simconnect.RequestDataOnSimObjectType(DATA_REQUESTS.REQUEST_1, DEFINITIONS.Struct1, 0, SIMCONNECT_SIMOBJECT_TYPE.USER);
            Console.WriteLine("Request sent...");
        }

        // Set up all the SimConnect related data definitions and event handlers
        private void initDataRequest()
        {
            try
            {
                // listen to connect and quit msgs
                simconnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(simconnect_OnRecvOpen);
                simconnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(simconnect_OnRecvQuit);

                // listen to exceptions
                simconnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(simconnect_OnRecvException);

                // define a data structure
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "Title", null, SIMCONNECT_DATATYPE.STRING256, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM ACTIVE FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM STANDBY FREQUENCY:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM ACTIVE FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simconnect.AddToDataDefinition(DEFINITIONS.Struct1, "COM STANDBY FREQUENCY:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);

                // COM1 Controls
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM_RADIO_WHOLE_INC, "COM_RADIO_WHOLE_INC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM_RADIO_WHOLE_DEC, "COM_RADIO_WHOLE_DEC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM_RADIO_FRACT_INC, "COM_RADIO_FRACT_INC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM_RADIO_FRACT_DEC, "COM_RADIO_FRACT_DEC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM_STBY_RADIO_SWAP, "COM_STBY_RADIO_SWAP");

                // COM2 controls
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM2_RADIO_WHOLE_INC, "COM2_RADIO_WHOLE_INC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM2_RADIO_WHOLE_DEC, "COM2_RADIO_WHOLE_DEC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM2_RADIO_FRACT_INC, "COM2_RADIO_FRACT_INC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM2_RADIO_FRACT_DEC, "COM2_RADIO_FRACT_DEC");
                simconnect.MapClientEventToSimEvent(EVENT_IDS.COM2_RADIO_SWAP, "COM2_RADIO_SWAP");

                // IMPORTANT: register it with the simconnect managed wrapper marshaller
                // if you skip this step, you will only receive a uint in the .dwData field.
                simconnect.RegisterDataDefineStruct<Struct1>(DEFINITIONS.Struct1);

                simconnect.OnRecvSimobjectData += new SimConnect.RecvSimobjectDataEventHandler(simconnect_OnRecvSimobjectData);

                // catch a simobject data request
                //simconnect.OnRecvSimobjectDataBytype += new SimConnect.RecvSimobjectDataBytypeEventHandler(simconnect_OnRecvSimobjectDataBytype);

                this.startRequest();
            }
            catch (COMException ex)
            {
                Console.WriteLine("Exception " + ex.Message);
            }
        }

        void simconnect_OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {
            Console.WriteLine("Got SimobjectData");

            switch ((DATA_REQUESTS)data.dwRequestID)
            {
                case DATA_REQUESTS.REQUEST_1:
                    Struct1 s1 = (Struct1)data.dwData[0];

                    Console.WriteLine("Title: " + s1.title);
                    Console.WriteLine("COM1:  " + s1.com1freq);
                    Console.WriteLine("COM2:  " + s1.com2freq);

                    form1.setFrequencies(s1.com1freq, s1.com1standby, s1.com2freq, s1.com2standby);
                    form1.updateSerialDisplay();
                    break;

                default:
                    Console.WriteLine("Unknown request ID: " + data.dwRequestID);
                    break;
            }
        }

        public void changeFreqMhz(int radio, int direction)
        {
            if (radio == 1)
            {
                if (direction > 0)
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM_RADIO_WHOLE_INC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
                else
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM_RADIO_WHOLE_DEC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
            }
            else
            {
                if (direction > 0)
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM2_RADIO_WHOLE_INC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
                else
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM2_RADIO_WHOLE_DEC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
            }
        }

        public void changeFreqKhz(int radio, int direction)
        {
            if (radio == 1)
            {
                if (direction > 0)
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM_RADIO_FRACT_INC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
                else
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM_RADIO_FRACT_DEC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
            }
            else
            {
                if (direction > 0)
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM2_RADIO_FRACT_INC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
                else
                {
                    simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM2_RADIO_FRACT_DEC, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
                }
            }
        }

        public void sendFreqSwap(int comradio)
        {
            if (comradio == 1)
            {
                simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM_STBY_RADIO_SWAP, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
            }
            else
            {
                simconnect.TransmitClientEvent(SimConnect.SIMCONNECT_OBJECT_ID_USER, EVENT_IDS.COM2_RADIO_SWAP, 0, GROUPS.GROUP1, SIMCONNECT_EVENT_FLAG.GROUPID_IS_PRIORITY);
            }
        }
    }
}
