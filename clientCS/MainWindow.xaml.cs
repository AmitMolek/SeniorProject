using System.Windows;
using WebCamService;
using System.IO;
namespace clientCS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        FtpClient ftp;
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
           this.ftp = new FtpClient();
            ftp.Login();
            //ftp.Upload(@"C:\לימודים\פרוייקט סוף\image1.jpg");
            //   ftp.Close();
        }
        private void btnSendFile_Click(object sender, RoutedEventArgs e)
        {
            this.ftp.Upload(@"C:\לימודים\פרוייקט סוף\image1.jpg");
            //   ftp.Close();
        }


        private void TextBox_DragEnter(object sender, DragEventArgs e)
        {
           // e.Effects = DragDropEffects.Copy;

           

            e.Handled = true;
        }

        private void TextBox_Drop(object sender, DragEventArgs e)
        {
            string[] fileloadup = (string[])e.Data.GetData(DataFormats.FileDrop);//Get the filename including path
            txtPath.Text = File.ReadAllText(fileloadup[0]);//Load data to textBox2

        }

        private void TextBox_Leave(object sender, DragEventArgs e)
        {
            e.Handled = true;
        }
    }
}
