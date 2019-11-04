using System.Windows;
using WebCamService;

namespace clientCS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {


        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            FtpClient ftp = new FtpClient();
            ftp.Login();
            ftp.Upload(@"C:\לימודים\פרוייקט סוף\image1.jpg");
            //   ftp.Close();
        }
    }
}
