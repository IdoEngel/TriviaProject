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
    /// Interaction logic for Sign.xaml
    /// </summary>
    public partial class Sign : Page
    {
    
        private Connect _server;
    
        public Sign(Connect server)
        {
            InitializeComponent();

            this._server = server;
        }

        private void LoginBtn_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Login(_server));

        }

        private void SignUpBtn_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new SignUp(_server));
        }


        private void GoBackBtn_Click(object sender, RoutedEventArgs e)
        {

        }


    }
}
