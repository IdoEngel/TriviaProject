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
using System.Windows.Threading;

namespace GUI.Forms
{
    /// <summary>
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Page
    {
        private Connect _server;
        private List<RoomInfo> availableRooms = new List<RoomInfo>();
        

        private DispatcherTimer refreshTimer;

        public JoinRoom(Connect server)
        {
            InitializeComponent();
            _server = server;
            LoadAvailableRooms();

            //start the timer
            refreshTimer = new DispatcherTimer();
            refreshTimer.Interval = TimeSpan.FromSeconds(3);
            refreshTimer.Tick += (s, e) => LoadAvailableRooms();
            refreshTimer.Start();

            this.Unloaded += (s, e) => refreshTimer?.Stop();

        }



        /// <summary>
        /// Loads the list of available rooms from the server and updates the UI with the retrieved data.
        /// </summary>
        /// <remarks>This method sends a request to the server to retrieve the list of available rooms. 
        /// The response is parsed, and the room information is stored in a collection, which is then  used to populate
        /// the UI element displaying the list of rooms.</remarks>
        private void LoadAvailableRooms()
        {
            byte[] header = new byte[5];
            header[0] = (int)ClientCodes.GET_ROOMS_REQUEST; // GET_ROOMS_REQUEST opcode
            Array.Clear(header, 1, 4); // empty JSON body 0 length

            _server.Send(header);

            string response = _server.Receive();
            JObject json = JObject.Parse(response);

            var rooms = json[serverFields.rooms._ROOMS_ARR];
            if (rooms is JObject roomObj)
            {
                foreach (var room in roomObj)
                {
                    //show only rooms that are not active and can be joined
                    //if (!(bool)room.Value[serverFields.rooms.IS_ACTIVE])
                    {
                        RoomInfo info = new RoomInfo
                        {
                            RoomName = room.Key,
                            RoomId = (int)room.Value[serverFields.rooms.ID]
                        };
                        availableRooms.Add(info);
                    }
                    
                }

                RoomsListBox.ItemsSource = availableRooms;
                RoomsListBox.DisplayMemberPath = "RoomName";
            }
        }

        private void JoinBtn_Click(object sender, RoutedEventArgs e)
        {
            var selectedRoom = (RoomInfo)RoomsListBox.SelectedItem;
            if (selectedRoom == null)
            {
                MessageBox.Show("Please select a room.");
                return;
            }

            var joinRequest = new
            {
                id = selectedRoom.RoomId, // correct field name
                username = _server.username
            };

            string json = JsonConvert.SerializeObject(joinRequest);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); // server expects big-endian

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.JOIN_ROOM_REQUEST; //correct opcode: JOIN_ROOM_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg);

            string response = _server.Receive();
            JObject jsonResp = JObject.Parse(response);

            try
            {
                int code = (int)jsonResp[serverFields.general.CODE];
                if (code == (int)Codes.JOIN_ROOM_SUCCESS)
                {
                    MessageBox.Show("Successfully joined room: " + selectedRoom.RoomName);
                    // TODO: navigate to room lobby
                    NavigationService.Navigate(new Room(_server, false));
                }
                else
                {
                    MessageBox.Show("Failed to join room. Maybe the room is active?");
                }
            }
            catch (ArgumentNullException msg)
            {
                MessageBox.Show("Failed to join room. Maybe the room is active?");
            }
        }



        private class RoomInfo
        {
            public string RoomName { get; set; }
            public int RoomId { get; set; }
        }


        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.MainFrame.Content = null;
            mainWindow.mainGrid.Visibility = Visibility.Visible;
        }

    }
}
