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
    public partial class Game : Page
    {
        private Connect _server;
        private DispatcherTimer timer;
        private int timeLeft;
        private int questionIndex = 0;
        private int correctAnswers = 0;
        private int totalQuestions = 10;
        private int questionTime;

        private int currentCorrectAnswerId = 1;

        public Game(Connect server, uint time, uint questions, uint players, bool isAdmin)
        {
            InitializeComponent();
            _server = server;
            totalQuestions = (int)questions;
            questionTime = (int)time;
            StartNextQuestion();
        }

        /// <summary>
        /// Initiates the process to display the next question in the game.
        /// </summary>
        /// <remarks>This method retrieves the next question and its corresponding answers from the
        /// server,  updates the UI elements to display the question and answers, and resets the timer for the question.
        /// If all questions have been completed, the game ends.</remarks>
        private void StartNextQuestion()
        {
            if (questionIndex >= totalQuestions)
            {
                endGame();
                return;
                
            }

            _server.SendJson((int)ClientCodes.GET_QUESTION_REQUEST, new { });
            string responseStr = _server.Receive(); //no header 

            var response = JObject.Parse(responseStr);

            //if ((int)response["status"] != 30)
            //{
            //    MessageBox.Show("Failed to get question.");
            //    return;
            //}

            string questionText = (string)response[serverFields.game.QUESTION];
            var answers = (JObject)response[serverFields.game._ANSWERS_ARR];
            var btns = new[] { AnswerBtn1, AnswerBtn2, AnswerBtn3, AnswerBtn4 };

            QuestionText.Text = questionText;
            for (int i = 1; i <= 4; i++) //question index starts with 1, 4 answers
            {
                btns[i-1].Content = answers[i.ToString()]?.ToString();
                btns[i-1].Tag = i.ToString();
            }
            

            timer?.Stop();
            StartTimer();
            UpdateStats();
        }

        /// <summary>
        /// Starts a countdown timer that decrements the remaining time at one-second intervals.
        /// </summary>
        /// <remarks>The timer initializes with a predefined duration and updates the UI to display the
        /// remaining time. When the timer reaches zero, it stops automatically and triggers the submission of an
        /// answer.</remarks>
        private void StartTimer()
        {
            timeLeft = questionTime;
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += (s, e) =>
            {
                timeLeft--;
                TimerText.Text = $"{timeLeft}";

                if (timeLeft <= 0)
                {
                    timer.Stop();
                    SubmitAnswer(1);
                }
            };
            timer.Start();
        }

        /// <summary>
        /// Submits the selected answer to the server and processes the server's response.
        /// </summary>
        /// <remarks>This method sends the selected answer to the server in a serialized JSON format.  The
        /// server's response is then parsed to determine whether the submitted answer is correct.  If the answer is
        /// correct, the internal correct answer count is incremented.  The method also advances the question index and
        /// initiates the next question.</remarks>
        /// <param name="selectedId">The ID of the selected answer to be submitted.</param>
        private void SubmitAnswer(int selectedId)
        {
            var answerObj = new 
                {
                    ansID = selectedId
                };



            string json = JsonConvert.SerializeObject(answerObj);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); 

            byte[] fullMsg = new byte[1 + 4 + jsonBytes.Length];
            fullMsg[0] = (int)ClientCodes.SUBMIT_ANS_REQUEST; //SUBMIT_ANSWER_REQUEST
            Array.Copy(lengthBytes, 0, fullMsg, 1, 4);
            Array.Copy(jsonBytes, 0, fullMsg, 5, jsonBytes.Length);

            _server.Send(fullMsg); //send it raw 

            string responseStr = _server.Receive();
            var response = JObject.Parse(responseStr);

            if ((int)response[serverFields.general.CODE] == (int)Codes.SUBMIT_ANS_SUCCESS)
            {
                int correctId = (int)response[serverFields.game.CORRECT_ANS_ID];
                if (selectedId == correctId)
                    correctAnswers++;
            }

            questionIndex++;
            StartNextQuestion();
        }

        /// <summary>
        /// update the stats on the display
        /// </summary>
        private void UpdateStats()
        {
            statsText.Text = $"correct: {correctAnswers} / {questionIndex + 1} of {totalQuestions}";
        }

        private void AnswerBtn_Click(object sender, RoutedEventArgs e)
        {
            timer.Stop();
            int selectedId = int.Parse((string)((Button)sender).Tag);
            SubmitAnswer(selectedId);
        }

        private void ExitBtn_Click(object sender, RoutedEventArgs e)
        {
            timer.Stop();
            _server.SendJson((int)ClientCodes.LEAVE_GAME_REQUEST, new { });
            _server.Receive();
            NavigationService.GoBack();
        }


        private void endGame()
        {
            NavigationService.Navigate(new gameSummery(_server));
        }

    }
}
