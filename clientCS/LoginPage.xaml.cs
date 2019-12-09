using client;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
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

namespace clientCS {
    /// <summary>
    /// Interaction logic for LoginPage.xaml
    /// </summary>
    public partial class LoginPage : Window {

        const int waitForConnectionMs = 10000;
        const string serverIP = "127.0.0.1";
        const int dataPort = 23456;
        const int instructionPort = 23457;
        WindowShare share;

        public LoginPage() {
            share = new WindowShare();

            InitializeComponent();
        }

        private async void btnConnect_Click(object sender, RoutedEventArgs e) {
            string connectionStr = "Connecting";
            string username = txtUserName.Text;
            lblWaitingConnection.Content = connectionStr;

            var cts = new CancellationTokenSource();

            cts.CancelAfter(waitForConnectionMs);
            Task<Tuple<bool, BaseClient>> conTask = Task<Tuple<bool, BaseClient>>.Factory.StartNew(() => {
                try {
                    BaseClient connection = new BaseClient(serverIP, dataPort, instructionPort, username);

                    while (!connection.getDataSocket().Connected) ;

                    return Tuple.Create(true, connection);
                } catch (Exception ex) {
                    Console.WriteLine(ex.Message);
                    return Tuple.Create(false, new BaseClient());
                }
            }, cts.Token);

            DispatcherTimer waitLblTimer = new DispatcherTimer();
            waitLblTimer.Interval = TimeSpan.FromMilliseconds(250);
            int dotCount = 1;
            waitLblTimer.Tick += (sender1, args) => {
                lblWaitingConnection.Content = connectionStr;
                for (int i = 0; i < dotCount; i++)
                    lblWaitingConnection.Content += ".";
                dotCount = (dotCount + 1) % 4;
            };
            waitLblTimer.Start();
            try {
                var conn = await conTask;

                if (conn.Item1) {
                    share.connection = conn.Item2;
                    MainWindow main = new MainWindow(ref share);
                    main.Show();
                    waitLblTimer.Stop();
                    this.Close();
                } else {
                    waitLblTimer.Stop();
                    lblWaitingConnection.Content = "Failed to connect";
                }
            } catch (Exception ex) {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
