using System.Windows;
using client;
using System.IO;
using System.Threading;
using System;
using Microsoft.Win32;

namespace clientCS
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        static  BaseClient ftp;
        public MainWindow()
        {
            InitializeComponent();
        }

        

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            ftp = new BaseClient("127.0.0.1", 23456, 23457,txtUserName.Text);
            while (ftp.getDataSocket().Connected == false) ;
           
            lblConnectionStatus.Content = "connected";
            
        }
       
        private void btnSendFile_Click(object sender, RoutedEventArgs e)
        {
            string filePath = txtPath.Text;
           ftp.sendFile(filePath);
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
            txtPath.Text = fileloadup[0];//Load data to textBox2

        }

        private void TextBox_Leave(object sender, DragEventArgs e)
        {
            e.Handled = true;
        }

        private void btnBrowse_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
                txtPath.Text = openFileDialog.FileName;
        }

     
    }
}
