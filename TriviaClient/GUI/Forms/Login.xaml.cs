using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Linq;

namespace GUI.Forms
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : Page
    {
        private Connect _server;
        public Login(Connect server)
        {
            InitializeComponent();
            _server = server;
        }

        private void LoginBtn_Click(object sender, RoutedEventArgs e)
        {
            error.Text = " ";
            string name = username.Text;
            string password = Password_field.Password;


            string response = SendLoginReq(name, password);

            //is the response code mean seccess
            if (response == Convert.ToString((int)Codes.LOG_IN_SUCCESS))
            {
                _server.IsLogged = true;
                _server.username = name;
                MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
                mainWindow.MainFrame.Content = null;
            }
            else
            {
                error.Text = response;
                SendLogoutReq(name);
            }
        }

        /// <summary>
        /// login request sender
        /// </summary>
        /// <param name="username">the username to login</param>
        /// <param name="password">the password of the user</param>
        /// <returns>if seccessful - return the opcode of secceess,
        /// if faild - return the error msg</returns>
        private string SendLoginReq(string username, string password)
        {
            var loginObj = new
            {
                username = username,
                password = password
            };

            string json = JsonConvert.SerializeObject(loginObj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if(BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); // convert to big-endian like the Python version

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.LOG_IN_REQUEST; //LOG_IN_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);


            _server.Send(fullMsg); // needs to support byte[] in Connect
            string res = _server.Receive();
            string response = _server.GetFieldValue(res, serverFields.general.CODE);

            if (response is null ||
                (!(response is null) && response.Equals("9")))
            {
                response = _server.GetFieldValue(res, serverFields.general.ERR_MSG);
            }

            return response;

            
        }

        /// <summary>
        /// a logout request sender
        /// </summary>
        /// <param name="username">the user to log out</param>
        private void SendLogoutReq(string username)
        {
            var logOutObj = new
            {
                username = username,
            };

            string json = JsonConvert.SerializeObject(logOutObj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); // convert to big-endian like the Python version

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.LOG_OUT_REQUEST; //LOG_OUT_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg); // needs to support byte[] in Connect
            string response = _server.GetFieldValue(_server.Receive(), serverFields.general.CODE);
        }



        private void Password_field_PasswordChanged(object sender, RoutedEventArgs e)
        {
            
        }

        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.GoBack();
        }
    }
}
