using GUI.Forms;
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
using System.Threading;
using System.ComponentModel;
using Newtonsoft.Json;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.StartPanel;


namespace GUI
{

    namespace serverFields
    {
        class general
        {
            //Shift+F12
            public const string ERR_MSG = "msg"; //general error

            public const string CODE = "code"; //general code

            //players in room data & top 5 high score 
            public const string _PLYAERS_ARR = "players"; //players in room data & top 5 high score & results of games
        }

        class rooms
        {
            public const string _ROOMS_ARR = "rooms"; //room data
            public const string ID = "id"; //room data
            public const string MAX_PLAYERS = "maxPlayers"; //room data
            public const string NUM_OF_QUESTIONS = "questionCount"; //room data
            public const string TIME_PER_QUSTION = "questionTimer"; //room data
            public const string IS_ACTIVE = "isActive"; //room data
            public const string _ROOM_DATA_OBJECT = "data"; //room data
            public const string LEFT_PLAYER = "leftMember"; //room data
        }

        //statistics data
        class statistics
        {
            public const string AVE_ANS_TIME = "aveAnswerTime"; //statistics data
            public const string CORRECT_ANSWERS_COUNT = "correctAnswersCount"; //statistics data
            public const string ANSWER_COUNT = "totalAnswersCount"; //statistics data
            public const string GAME_COUNT = "gameCount"; //statistics data
            public const string SCORE = "score"; //statistics data
        }

        class game
        { //game data
            public const string QUESTION = "question";
            public const string _ANSWERS_ARR = "answers"; //object as key-value [ans number-ans]
            public const string CORRECT_ANS_ID = "correctAnsID";
            public const string IS_SUCCESSFUL = "isSuccessful"; //is the call to GameResult is passible (is game over?)
            public const string CORRECT_ANS_COUNT = "correctAnsCount";
            public const string WRONG_ANS_COUNT = "wrongAnsCount";
            public const string AVE_ANS_TIME = "aveAnsTime";
        }

    }

    class fieldNames
    { //names of fields on the protocol - from the client
      //login, signup logout - from client
        public const string KEY_USERNAME = "username"; //login, signup logout
        public const string KEY_PASSWORD = "password"; //login, signup logout
        public const string KEY_EMAIL = "email"; //login, signup logout

        public const string KEY_ROOM_ID = "id"; //room requests
        public const string KEY_ROOM_NAME = "name"; //room requests
        public const string KEY_MAX_PLAYERS = "maxPlayers"; //room requests
        public const string KEY_NUM_OF_QUESTIONS = "numOfQuestions"; //room requests
        public const string KEY_TIME_PER_QUESTION = "timePerQuestion"; //room requests

        public const string KEY_ANS_ID = "ansID"; //game requests
    }
    /**
    * @brief server codes that sent to the client
    */
    enum Codes
    {
        GENERAL_ERROR = 9,

	    SIGN_UP_SUCCESS = 10,
	    LOG_IN_SUCCESS = 11,
	    LOG_OUT_SUCCESS = 12,
	    SIGN_UP_FAIL = 13,
	    LOG_IN_FAIL = 14,
	    LOG_OUT_FAIL = 15,

	    GET_ROOMS_SUCCESS = 16,
	    GET_PLAYERS_IN_ROOM_SUCCESS = 17,
	    GET_HIGN_SCORE_TOP_FIVE_SUCCESS = 18,
	    GET_PERSONAL_STATS_SUCCESS = 19,
	    JOIN_ROOM_SUCCESS = 20,
	    CREATE_ROOM_SUCCESS = 21,

	    CLOSE_ROOM_SUCCESS = 22,
	    START_GAME_SUCCESS = 23,
	    GET_ROOM_STATE_SUCCESS = 24,
	    LEAVE_ROOM_SUCCESS = 25,
	    _NOTIFY_CLOSED_ROOM = 26,
	    _NOTIFY_GAME_STARTED = 27,
	    _NOTIFY_ADMIN_MEMBER_LEFT = 28,

	    LEAVE_GAME_SUCCESS = 29,
	    GET_QUESTION_SUCCESS = 30,
	    SUBMIT_ANS_SUCCESS = 31,
	    GET_ROOM_RESULTS_SUCCESS = 32,
	    GET_ROOM_RESULTS_FAIL = 33,
    };

    /**
    * @brief client codes sent to the server
    */
    enum ClientCodes
    {
        GENERAL_ERROR = 50,
	    SIGN_UP_REQUEST = 51,
	    LOG_IN_REQUEST = 52,
	    LOG_OUT_REQUEST = 53,

	    PLAYERS_IN_ROOM_REQUEST = 54, ///
	    JOIN_ROOM_REQUEST = 55,
	    CREATE_ROOM_REQUEST = 56,
	    GET_HIGH_SCORE_REQUEST = 57,
	    GET_PRSONAL_STAT_REQUEST = 58,
	    GET_ROOMS_REQUEST = 59,

	    CLOSE_ROOM_REQUEST = 60,
	    START_GAME_REQUEST = 61,
	    GET_ROOM_STATE_REQUEST = 62,
	    LEAVE_ROOM_REQUEST = 63,
	    _NOTIFY_CLOSED_ROOM_sts = 64, //server-to-server code
	    _NOTIFY_GAME_STARTED_sts = 65, //server-to-server code
	    _NOTIFY_ADMIN_THAT_MEMBER_LEFT_sts = 66, //server-t-server code

	    LEAVE_GAME_REQUEST = 67,
	    GET_QUESTION_REQUEST = 68,
	    SUBMIT_ANS_REQUEST = 69,
	    GET_GAME_RESULTS_REQUEST = 70,
    };


    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        

        private Connect server;
        public MainWindow()
        {
            InitializeComponent();;

            this.server = new Connect();

            Sign SignPage = new Sign(server);//sign page

            StartPage start = new StartPage();//start page for cool start screen

            StartFrame.Navigate(start);//start frame for cool start screen

            MainFrame.Navigate(SignPage);//pass it to frame navigate method

        }
        
        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            JoinRoom JoinRoomPage = new JoinRoom(server);//object of the page

            MainFrame.Navigate(JoinRoomPage);//pass it to frame navigate method
        }
        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            CreateRoom CreateRoomPage = new CreateRoom(server);//object of the page 
            
            MainFrame.Navigate(CreateRoomPage);//pass it to frame navigate method
        }

        private void StatusBtn_Click(object sender, RoutedEventArgs e)
        {
            MyStatus MyStatusPage = new MyStatus(server);//object of the page 
            
            MainFrame.Navigate(MyStatusPage);//pass it to frame navigate method
        }

        private void ScoresBtn_Click(object sender, RoutedEventArgs e)
        {
            BestScores BestScoresPage = new BestScores(server);//object of the page 
            
            MainFrame.Navigate(BestScoresPage);//pass it to frame navigate method
        }

        private void MainFrame_Navigated(object sender, NavigationEventArgs e)
        {

        }

        private void StartFrame_Navigated(object sender, NavigationEventArgs e)
        {

        }

        private void logOutBtn_Click(object sender, RoutedEventArgs e)
        {
            var logOutObj = new
            {
                username = server.username,
            };

            string json = JsonConvert.SerializeObject(logOutObj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); 

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.LOG_OUT_REQUEST; 
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            server.Send(fullMsg); 
            string response = server.GetFieldValue(server.Receive(), serverFields.general.CODE);


            Sign SignPage = new Sign(server);

            MainFrame.Navigate(SignPage);
        }
    }
}
