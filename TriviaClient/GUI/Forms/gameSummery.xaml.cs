using Newtonsoft.Json.Linq;
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
    /// Interaction logic for gameSummery.xaml
    /// </summary>
    public partial class gameSummery : Page
    {
        private Connect _server;


        public class PlayerSummary
        {
            public string Username { get; set; }
            public int CorrectAnsCount { get; set; }
            public int WrongAnsCount { get; set; }
            public float AveAnsTime { get; set; }
        }


        public gameSummery(Connect _server)
        {
            InitializeComponent();
            this._server = _server;

            initSummery();

        }


        /// <summary>
        /// Initializes and populates the game summary by retrieving game results from the server.
        /// </summary>
        /// <remarks>This method sends a request to the server to fetch the game results and processes the
        /// response. If the request is successful, it parses the player statistics and updates the summary view. If the
        /// request fails, an appropriate error message is displayed to the user.</remarks>
        private void initSummery()
        {
            bool isSuccessful = false;
            string responseStr;
            JObject res = new JObject{ };

            while (!isSuccessful)
            {
                _server.SendJson((int)ClientCodes.GET_GAME_RESULTS_REQUEST, new { });

                responseStr = _server.Receive();

                res = JObject.Parse(responseStr);
                isSuccessful = (bool)res[serverFields.game.IS_SUCCESSFUL];
            }

            if ((int)res[serverFields.general.CODE] == (int)Codes.GET_ROOM_RESULTS_SUCCESS && 
                isSuccessful)
            {
                var playersObj = res[serverFields.general._PLYAERS_ARR] as JObject;
                var summaries = new List<PlayerSummary>();

                foreach (var player in playersObj)
                {
                    string username = player.Key;
                    var stats = player.Value;

                    summaries.Add(new PlayerSummary
                    {
                        Username = username,
                        CorrectAnsCount = (int)stats[serverFields.game.CORRECT_ANS_COUNT],
                        WrongAnsCount = (int)stats[serverFields.game.WRONG_ANS_COUNT],
                        AveAnsTime = (float)stats[serverFields.game.AVE_ANS_TIME]
                    });
                }

                summary.ItemsSource = summaries;
            }
            else
            {
                MessageBox.Show($"ServerError: {res[serverFields.general.ERR_MSG]}");
            }
        }

        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.MainFrame.Content = null;
        }
    }
}
