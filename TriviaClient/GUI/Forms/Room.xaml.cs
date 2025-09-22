using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
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
using static System.Windows.Forms.VisualStyles.VisualStyleElement.TaskbarClock;

namespace GUI.Forms
{
    public partial class Room : Page
    {
        private Connect _server;
        private bool _isAdmin;
        private Thread updateThread;
        private bool keepUpdating = true;

        private uint questionTime = 30;
        private uint questionCount = 10;
        private uint maxPlayers = 0;

        public Room(Connect server, bool isAdmin)
        {
            InitializeComponent();
            _server = server;
            _isAdmin = isAdmin;

            startBtn.Visibility = isAdmin ? Visibility.Visible : Visibility.Hidden;// making the btn visable and usable only to admins
            closeBtn.Visibility = isAdmin ? Visibility.Visible : Visibility.Hidden;// making the btn visable and usable only to admins


            StartUpdateThread();
        }

        /// <summary>
        /// check if the game started. move to the right page on command
        /// </summary>
        private void StartUpdateThread()
        {
            updateThread = new Thread(() =>
            {
                while (keepUpdating)
                {
                    try
                    {
                        SendGetRoomState();

                        string msg = _server.Receive();
                        var json = JObject.Parse(msg);
                        
                        if ((int)json[serverFields.general.CODE] == (int)Codes._NOTIFY_GAME_STARTED) //game started
                        {
                            Dispatcher.Invoke(() =>
                            {
                                keepUpdating = false;
                                AbortUpdater();
                                NavigationService.Navigate(new Game(_server, questionTime, questionCount, maxPlayers, _isAdmin));
                            });
                            break;
                        }
                        else if ((int)json[serverFields.general.CODE] == (int)Codes._NOTIFY_CLOSED_ROOM) //room closed
                        {
                            Dispatcher.Invoke(() =>
                            {
                                MessageBox.Show("Room has been closed.");
                                keepUpdating = false;
                                AbortUpdater();
                                NavigationService.GoBack();
                            });
                            break;
                        }
                    }
                    catch { }

                    Thread.Sleep(3000);
                }
            });
            updateThread.IsBackground = true;
            updateThread.Start();
        }

        /// <summary>
        /// update visualy the room state, ask the server for the room state
        /// </summary>
        private void SendGetRoomState()
        {
            var json = Encoding.UTF8.GetBytes("{}");
            byte[] lengthBytes = BitConverter.GetBytes(json.Length);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);

            byte[] msg = new byte[1 + 4 + json.Length];
            msg[0] = (int)ClientCodes.GET_ROOM_STATE_REQUEST; // GET_ROOM_STATE_REQUEST
            Array.Copy(lengthBytes, 0, msg, 1, 4);
            Array.Copy(json, 0, msg, 5, json.Length);

            _server.Send(msg);
            string responseStr = _server.Receive();

            var res = JObject.Parse(responseStr);
            
            if ((int)res[serverFields.general.CODE] == (int)Codes.GET_ROOM_STATE_SUCCESS)
            {
                var data = res[serverFields.rooms._ROOM_DATA_OBJECT];
                var playerList = res[serverFields.general._PLYAERS_ARR].ToObject<List<string>>();

                questionTime = (uint)data[serverFields.rooms.TIME_PER_QUSTION];
                questionCount = (uint)data[serverFields.rooms.NUM_OF_QUESTIONS];
                maxPlayers = (uint)playerList.Count;

                Dispatcher.Invoke(() =>
                {
                    players.ItemsSource = playerList;
                    timePerQ.Content = $"{questionTime}";
                    numOfQ.Content = $"{questionCount}";
                    maxPlayersLabel.Content = $"{maxPlayers}";
                });
            }
        }

        private void startBtn_Click(object sender, RoutedEventArgs e)
        {
            keepUpdating = false;
            AbortUpdater();

            SendSimpleRequest((int)ClientCodes.START_GAME_REQUEST); // START_GAME_REQUEST

            string response = _server.RawReceive(); //
            

            NavigationService.Navigate(new Game(_server, questionTime, questionCount, maxPlayers, _isAdmin));
        }



        private void leaveBtn_Click(object sender, RoutedEventArgs e)
        {
            SendSimpleRequest((int)ClientCodes.LEAVE_ROOM_REQUEST); // LEAVE_ROOM_REQUEST
            string responseStr = _server.Receive();
            var res = JObject.Parse(responseStr);

            if ((int)res[serverFields.general.CODE] == (int)Codes.LEAVE_ROOM_SUCCESS)
            {
                keepUpdating = false;
                AbortUpdater();
                NavigationService.GoBack();
            }
            else
            {
                MessageBox.Show($"failed to leave room - {res[serverFields.general.ERR_MSG]}");
            }
        }

        private void closeBtn_Click(object sender, RoutedEventArgs e)
        {
            if (!_isAdmin) return;

            SendSimpleRequest((int)ClientCodes.CLOSE_ROOM_REQUEST); // CLOSE_ROOM_REQUEST
            
            keepUpdating = false;
            AbortUpdater();
            NavigationService.GoBack();
            
        }

        /// <summary>
        /// send a request that gets only an opcode and return only an opcode in response
        /// </summary>
        /// <param name="code"> the opcode of the request</param>
        private void SendSimpleRequest(byte code)
        {
            var json = Encoding.UTF8.GetBytes("{}");
            byte[] lengthBytes = BitConverter.GetBytes(json.Length);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);

            byte[] msg = new byte[1 + 4 + json.Length];
            msg[0] = code;
            Array.Copy(lengthBytes, 0, msg, 1, 4);
            Array.Copy(json, 0, msg, 5, json.Length);

            _server.Send(msg);
        }

        private void AbortUpdater()
        {
            try { updateThread?.Interrupt(); } catch { }
            try { updateThread?.Abort(); } catch { }
        }



        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            if(_isAdmin)
            {
                SendSimpleRequest((int)ClientCodes.CLOSE_ROOM_REQUEST); //CLOSE_ROOM_REQUEST
                
                keepUpdating = false;
                AbortUpdater();
                NavigationService.GoBack();
                
            }
            else
            {
                SendSimpleRequest((int)ClientCodes.LEAVE_GAME_REQUEST); //LEAVE_GAME_REQUEST

                keepUpdating = false;
                AbortUpdater();
                NavigationService.GoBack();
            }
        }


    }
}