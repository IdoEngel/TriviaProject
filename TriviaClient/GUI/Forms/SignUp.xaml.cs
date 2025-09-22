using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
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
    /// Interaction logic for SignUp.xaml
    /// </summary>
    public partial class SignUp : Page
    {
        private Connect _server;

        public SignUp(Connect server)
        {
            InitializeComponent();
            _server = server;
        }

        private void SignUpBtn_Click(object sender, RoutedEventArgs e)
        {
            error.Text = " ";
            string name = username.Text;
            string password = Password_field.Password;
            string email = mail.Text;


            Tuple<string, string?> response = SendSignUpReq(name, password, email);
            //MessageBox.Show(response);

            if (response.Item1 == Convert.ToString((int)Codes.SIGN_UP_SUCCESS))
            {
                _server.IsLogged = true;
                _server.username = name;
                MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;


                //clear the frame
                mainWindow.MainFrame.Content = null;
                mainWindow.mainGrid.Visibility = Visibility.Visible;
            }
            else
            {
                error.Text = $"ERORR! : {(response.Item2 is null ? "unknown" : response.Item2)}";
                SendLogoutReq(name);
            }

        }

        /*
         * first - code
         * second - error msg if exists
         */

        /// <summary>
        /// sends a signup request
        /// </summary>
        /// <param name="username">the username to sign</param>
        /// <param name="password">the password of the user</param>
        /// <param name="email">the email of the user</param>
        /// <returns>
        /// Tuple of two params
        /// First - the opcode
        /// Second - the error msg (if not exists - null)</returns>
        private Tuple<string, string?> SendSignUpReq(string username, string password, string email)
        {
            var signUpObj = new
            {
                username = username,
                password = password,
                email = email
            };

            string json = JsonConvert.SerializeObject(signUpObj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); 

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.SIGN_UP_REQUEST; // SIGN_UP_REQUEST 
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);
            _server.Send(fullMsg);

            string res = _server.Receive();
            string response = _server.GetFieldValue(res , serverFields.general.CODE);

            return Tuple.Create(response, (int.Parse(response) == (int)Codes.SIGN_UP_SUCCESS ? (string)null : 
                _server.GetFieldValue(res, serverFields.general.ERR_MSG)));

        }

        private void Password_field_PasswordChanged(object sender, RoutedEventArgs e)
        {

        }

        /// <summary>
        /// sends a logout request (in case of fail to signup
        /// </summary>
        /// <param name="username">the username to logout</param>
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
                Array.Reverse(lengthBytes); 

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.LOG_OUT_REQUEST; //LOG_OUT_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg); 
            string response = _server.GetFieldValue(_server.Receive(), serverFields.general.CODE);
        }
        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.GoBack();
        }
    }

}
