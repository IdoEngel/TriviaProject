using Newtonsoft.Json.Linq;
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

namespace GUI.Forms
{
    /// <summary>
    /// Interaction logic for CreateRoom.xaml
    /// </summary>
    public partial class CreateRoom : Page
    {
        private Connect _server;

        public CreateRoom(Connect server)
        {
            InitializeComponent();
            _server = server;
        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            string roomName = RoomNameBox.Text;

            uint maxPlayers = (uint)MaxPlayersSlider.Value;
            uint numQuestions = (uint)QuestionCountSlider.Value;
            uint timePerQuestion = (uint)TimePerQSlider.Value;

            if (string.IsNullOrWhiteSpace(roomName))
            {
                MessageBox.Show("please enter a room name.");
                return;
            }

            var request = new
            {
                name = roomName,
                username = _server.username,
                maxPlayers = maxPlayers,
                numOfQuestions = numQuestions,
                timePerQuestion = timePerQuestion
            };



            string json = JsonConvert.SerializeObject(request);
            
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); 

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.CREATE_ROOM_REQUEST; // CREATE_ROOM_REQUEST opcode
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);


            _server.Send(fullMsg);

            string response = _server.Receive();
            JObject jsonResp = JObject.Parse(response);

            int code = (int)jsonResp[serverFields.general.CODE];
            if (code == (int)Codes.CREATE_ROOM_SUCCESS)
            {
                MessageBox.Show("room created successfully.");
                NavigationService.Navigate(new Room(_server, true));
            }
            else
            {
                MessageBox.Show("Failed to create room.");
            }
        }


        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.MainFrame.Content = null;
            mainWindow.mainGrid.Visibility = Visibility.Visible;
        }


    }
}
