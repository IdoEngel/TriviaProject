using Newtonsoft.Json;
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
using static System.Collections.Specialized.BitVector32;

namespace GUI.Forms
{
    /// <summary>
    /// Interaction logic for MyStatus.xaml
    /// </summary>
    /// 
    public partial class MyStatus : Page
    {
        private Connect _server;

        
        //string response = SendStatusReq(username);

        public MyStatus(Connect server)
        {
            InitializeComponent();
            _server = server;

            string username = _server.username; // or however you store it
            string response = SendStatusReq(username);
            ShowStats(response);


        }

        /// <summary>
        /// send a personal stats request to the server
        /// </summary>
        /// <param name="username">the username to get its stats</param>
        /// <returns>a string that represents a json object - the answer from the server</returns>
        private string SendStatusReq(string username)
        {
            var obj = new { username = username };
            string json = JsonConvert.SerializeObject(obj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.GET_PRSONAL_STAT_REQUEST; // GET_PRSONAL_STAT_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg);
            return _server.Receive();
        }

        /// <summary>
        /// get a string that represents a json obj, read the json and print the stats to the user
        /// </summary>
        /// <param name="json">the json repr to show to the user</param>
        private void ShowStats(string json)
        {
            var root = JObject.Parse(json);
            var stats = root.Properties().FirstOrDefault(p => p.Name != serverFields.general.CODE)?.Value as JObject;

            string currKey = serverFields.statistics.SCORE;
            scoreLabel.Content = stats.ContainsKey(currKey) ? $"Score: {stats[currKey]}" : "N/A";

            currKey = serverFields.statistics.AVE_ANS_TIME;
            timeLabel.Content = stats.ContainsKey(currKey) ? $"Avg Time: {stats[currKey]}" : "N/A";

            currKey = serverFields.statistics.CORRECT_ANSWERS_COUNT;
            correctLabel.Content = stats.ContainsKey(currKey) ? $"Correct: {stats[currKey]}" : "N/A";

            currKey = serverFields.statistics.ANSWER_COUNT;
            totalLabel.Content = stats.ContainsKey(currKey) ? $"Total: {stats[currKey]}" : "N/A";

            currKey = serverFields.statistics.GAME_COUNT;
            gameLabel.Content = stats.ContainsKey(currKey) ? $"Games: {stats[currKey]}" : "N/A";
        }


        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.MainFrame.Content = null;
            mainWindow.mainGrid.Visibility = Visibility.Visible;
        }


    }
}
