using System.Windows;
using client;
using System.IO;
using System.Threading;

namespace clientCS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        BaseClient ftp;
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            Thread t = new Thread(btnConnect_click_thread);
            t.Start();
           
            //ftp.Upload(@"C:\לימודים\פרוייקט סוף\image1.jpg");
            //   ftp.Close();
        }
        private void btnConnect_click_thread()
        {
            this.ftp = new BaseClient("127.0.0.1", 23456, 23457);
        }
        private void btnSendFile_Click(object sender, RoutedEventArgs e)
        {
          // this.ftp.Upload(@"C:\לימודים\פרוייקט סוף\image1.jpg");
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
