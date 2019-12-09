using System.Windows;
using client;
using System.IO;
using System.Threading;
using System;
using Microsoft.Win32;
using System.Windows.Controls;
using System.Windows.Data;

namespace clientCS
{
    public class TestViewItem {
        public string fileName;
        public string fileSize;
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        //static  BaseClient ftp;
        BaseClient ftp;
        public MainWindow(ref WindowShare share)
        {
            ftp = share.connection;
            InitializeComponent();
        }

        

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            //ftp = new BaseClient("127.0.0.1", 23456, 23457,txtUserName.Text);
            //ftp = new BaseClient("10.0.0.100", 23456, 23457, txtUserName.Text);
            while (ftp.getDataSocket().Connected == false) ;
           
            //lblConnectionStatus.Content = "connected";
            
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

        private void TabControl_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e) {
            var tc = sender as TabControl;

            if (tc != null) {
                switch (tc.Name) {
                    case "tab_MyFiles":
                        Console.WriteLine("helloo world!");
                        GridView gv = new GridView();
                        listView_Files.View = gv;
                        gv.Columns.Add(new GridViewColumn { Header = "File Name", DisplayMemberBinding = new Binding("fileName")});
                        gv.Columns.Add(new GridViewColumn { Header = "File Size", DisplayMemberBinding = new Binding("fileSize") });
                        listView_Files.Items.Add(new TestViewItem { fileName = "Test", fileSize="54"});
                        listView_Files.Items.Add(new TestViewItem { fileName = "Test", fileSize = "54" });
                        listView_Files.Items.Add(new TestViewItem { fileName = "Test", fileSize = "54" });
                        break;
                }
            }
        }
    }
}
