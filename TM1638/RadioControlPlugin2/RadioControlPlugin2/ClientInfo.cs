using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace RadioControlPlugin2
{
    class ClientInfo
    {
        public UdpClient client = null;
        public IPEndPoint ipEndpoint = null;
        
        public ClientInfo(UdpClient c, IPEndPoint e)
        {
            this.client = c;
            this.ipEndpoint = e;
        }
    }
}
