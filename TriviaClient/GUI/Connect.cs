using System;
using System.Collections.Generic;
using System.Diagnostics.Eventing.Reader;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;



namespace GUI
{
    public class Connect
    {
        private TcpClient client;

        private NetworkStream stream;

        public string roomID;

        public string username;


        public bool IsLogged;
        public bool IsConnected => client?.Connected ?? false;

        public Connect(string host = "127.0.0.1", int port = 49153)
        {
            try
            {
                client = new TcpClient();
                client.Connect(host, port);
                stream = client.GetStream();
            }
            catch (SocketException ex)
            {
                //handle connection failure
                System.Windows.MessageBox.Show($"failed to connect: {ex.Message}");
            }
        }

        /// <summary>
        /// sends data to the server
        /// </summary>
        /// <param name="data">the data to send to the server</param>
        public void Send(byte[] data)
        {
            if (IsConnected)
            {
                stream.Write(data, 0, data.Length);
            }
        }

        /// <summary>
        /// gets the answer from the server
        /// </summary>
        /// <returns>a string of the json (cut header)</returns>
        /// <exception cref="Exception"> if the len of the json is more then passible in the protocol</exception>
        public string Receive()
        {
            if (!IsConnected) return null;

            byte[] header = ReadExact(9);
            byte status = header[0];

            int jsonLength =
                (int)header[1] +
                (int)header[2] +
                (int)header[3] +
                (int)header[4] +
                (int)header[5] +
                (int)header[6] +
                (int)header[7] +
                (int)header[8];

            if (jsonLength <= 0 || jsonLength > 127*8)
                throw new Exception("Invalid JSON length: " + jsonLength);

            byte[] jsonBuffer = ReadExact(jsonLength);
            return Encoding.UTF8.GetString(jsonBuffer);

           
        }

        /// <summary>
        /// returns the bytes from the server with the headers
        /// </summary>
        /// <returns>all the msg from the server</returns>
        public string RawReceive()
        {
            byte[] buffer = new byte[10000];
            int bytesRead = stream.Read(buffer, 0, buffer.Length);
            return Encoding.UTF8.GetString(buffer, 0, bytesRead);
        }


        private byte[] ReadExact(int length)
        {
            byte[] buffer = new byte[length];
            int totalRead = 0;

            while (totalRead < length)
            {
                int read = stream.Read(buffer, totalRead, length - totalRead);
                if (read == 0) throw new Exception("Connection closed during read.");
                totalRead += read;
            }

            return buffer;
        }

        /// <summary>
        /// gets a field from a msg that came from the client
        /// </summary>
        /// <param name="msg">the msg to get the field (must be json)</param>
        /// <param name="field">the field name to get </param>
        /// <returns></returns>
        public string? GetFieldValue(string msg, string field)
        {
            try
            {
                var json = JsonConvert.DeserializeObject<Dictionary<string, object>>(msg);
                return json.ContainsKey(field) ? json[field]?.ToString() : null;
            }
            catch
            {
                return null;
            }
        }


        public void Close()
        {
            stream?.Close();
            client?.Close();
        }

        ~Connect()
        {
            Close();
        }

    
        /// <summary>
        /// sends msg to the server, build the msg in-house
        /// </summary>
        /// <param name="code">the msg code of the msg to send</param>
        /// <param name="obj">a json object to send to the server ( new { } )</param>
        public void SendJson(byte code, object obj)
        {  
            string json = JsonConvert.SerializeObject(obj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);

            byte[] msg = new byte[1 + 4 + jsonBytes.Length];
            msg[0] = code;
            msg[1] = (byte)(jsonBytes.Length & 0xFF);
            msg[2] = (byte)((jsonBytes.Length >> 8) & 0xFF);
            msg[3] = (byte)((jsonBytes.Length >> 16) & 0xFF);
            msg[4] = (byte)((jsonBytes.Length >> 24) & 0xFF);


                Array.Copy(jsonBytes, 0, msg, 5, jsonBytes.Length);
            Send(msg);
        }

    }
}

