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
    /// Interaction logic for BestScores.xaml
    /// </summary>
    public partial class BestScores : Page
    {
        private Connect _server;

        public BestScores(Connect server)
        {
            InitializeComponent();
            _server = server;
            LoadBestScores();
        }

        /// <summary>
        /// Sends a request to the server to retrieve the top five high scores and updates the UI with the results.
        /// </summary>
        /// <remarks>This method communicates with the server to fetch the top five high scores, processes
        /// the server's response,  and updates the corresponding UI elements with the retrieved data. If the server
        /// response indicates failure  or cannot be parsed, an error message is displayed to the user.</remarks>
        private void LoadBestScores()
        {
            var obj = new { };
            string json = Newtonsoft.Json.JsonConvert.SerializeObject(obj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); // convert to big-endian

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.GET_HIGH_SCORE_REQUEST; // GET_HIGH_SCORE
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg);
            string responseStr = _server.Receive();

            try
            {
                var response = JObject.Parse(responseStr);
                if ((int)response[serverFields.general.CODE] != (int)Codes.GET_HIGN_SCORE_TOP_FIVE_SUCCESS)
                {
                    MessageBox.Show("Failed to load high scores.");
                    return;
                }

                var players = (JObject)response[serverFields.general._PLYAERS_ARR];
                var props = players.Properties().ToList();
                var scoreLabels = new[] { score1, score2, score3, score4, score5 };

                for (int i = 0; i < scoreLabels.Length; i++) 
                {
                    if (i < props.Count)
                    {
                        var stats = (JObject)props[i].Value;
                        scoreLabels[i].Content = 
                            $"{props[i].Name} - Games: {stats[serverFields.statistics.GAME_COUNT]}, " +
                            $"Score: {stats[serverFields.statistics.SCORE]}";
                    }
                    else
                    {
                        scoreLabels[i].Content =
                            "";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"error parsing server response: {ex.Message}");
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

