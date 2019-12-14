using System.Windows;
using client;
using System.IO;
using System.Threading;
using System;
using Microsoft.Win32;
using System.Windows.Controls;
using System.Windows.Data;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Diagnostics;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Threading;


namespace clientCS
{
    public class FilesViewItem {
        public string fileName { get; set; }
        public string fileSize { get; set; }
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        //static  BaseClient ftp;
        BaseClient ftp;
        public string[] files;
        TabItem lastTabItem;
        List<string> pathsToUpload;

        public MainWindow(ref WindowShare share)
        {
            ftp = share.connection;
            pathsToUpload = new List<string>();
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

            string connectionStr = "Uploading";
            lblFileUploadStatus.Visibility = Visibility.Visible;
            lblFileUploadStatus.Content = connectionStr;
            DispatcherTimer fileStatusLblTimer = new DispatcherTimer();
            fileStatusLblTimer.Interval = TimeSpan.FromMilliseconds(250);
            int dotCount = 1;
            fileStatusLblTimer.Tick += (sender1, args) => {
                lblFileUploadStatus.Content = connectionStr;
                for (int i = 0; i < dotCount; i++)
                    lblFileUploadStatus.Content += ".";
                dotCount = (dotCount + 1) % 4;
            };
            fileStatusLblTimer.Start();
            Task sendFile = Task.Factory.StartNew(() => { ftp.sendFile(filePath, ref fileStatusLblTimer); });
            Dispatcher.BeginInvoke(DispatcherPriority.Background ,new Action<Task>(async (task) => {
                await Task.Run(()=> {
                    task.Wait();
                });
                lblFileUploadStatus.Content = "Finished sending";
            }), sendFile);
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
            openFileDialog.Multiselect = true;
            openFileDialog.Title = "Select files to upload";
            if (openFileDialog.ShowDialog() == true) {
                Stream[] files = openFileDialog.OpenFiles();
                pathsToUpload.Clear();
                foreach (Stream s in files) {
                    pathsToUpload.Add(openFileDialog.FileName);
                }
                txtPath.Text = openFileDialog.FileName;
            }
        }

        private async void TabControl_SelectionChangedAsync(object sender, System.Windows.Controls.SelectionChangedEventArgs e) {
            var tc = sender as TabControl;

            if (tc != null) {
                if (e.Source is TabControl) {
                    TabItem item = tc.SelectedItem as TabItem;
                    Debug.WriteLine("Item: " + item.Name);
                    switch (item.Name) {
                        case "tab_MyFiles":
                            try {
                                if (item == lastTabItem)
                                    return;
                                ftp.sendGetListFiles();

                                CancellationTokenSource cts = new CancellationTokenSource();
                                cts.CancelAfter(5000);
                                Task<List<FilesViewItem>> waitForFiles = Task.Factory.StartNew(() => {
                                    try {
                                        Socket dataSoket = ftp.getDataSocket();
                                        while (dataSoket.Available < 0) {
                                            if (cts.IsCancellationRequested)
                                                throw new Exception("Timeout");
                                        }

                                        const int bufferSize = 1024;
                                        byte[] buffer = new byte[bufferSize];
                                        dataSoket.Receive(buffer);

                                        string result = System.Text.Encoding.ASCII.GetString(buffer);
                                        string resultNoCode = result.Substring(result.IndexOf(':', result.IndexOf(':') + 1) + 1);
                                        string[] tuples = resultNoCode.Split(',');
                                        List<FilesViewItem> filesList = new List<FilesViewItem>();
                                        foreach (string s in tuples) {
                                            string[] filesColInfo = s.Split('^');
                                            if (filesColInfo.Length > 1) {
                                                string fileName = (filesColInfo[0].Split(':')[1]);
                                                string fileSize = (filesColInfo[1].Split(':')[1]);
                                                filesList.Add(new FilesViewItem { fileName = fileName, fileSize = fileSize });
                                            }
                                        }

                                        return filesList;
                                    } catch (Exception ex) {
                                        Console.WriteLine(ex.Message);
                                        return new List<FilesViewItem>();
                                    }
                                }, cts.Token);

                                listView_Files.Items.Clear();
                                List<FilesViewItem> files = await waitForFiles;
                                foreach (var file in files) {
                                    if (!listView_Files.Items.Contains(file))
                                        listView_Files.Items.Add(file);
                                }
                            } catch (Exception ex) {
                                Console.WriteLine(ex.Message);
                            }
                            break;
                    }
                    lastTabItem = item;
                }
            }
        }

        private void btnBrowseFolder_Click(object sender, RoutedEventArgs e)
        {

            var dlg = new System.Windows.Forms.FolderBrowserDialog();
            System.Windows.Forms.DialogResult result = dlg.ShowDialog();
            if (!string.IsNullOrWhiteSpace(dlg.SelectedPath))
            {
                this.files = Directory.GetFiles(dlg.SelectedPath);

                System.Windows.Forms.MessageBox.Show("Files found: " + files.Length.ToString(), "Message");
            }

        }

        private void btnSendFolder_Click(object sender, RoutedEventArgs e)
        {
            foreach (string filePath in files)
            {

                string connectionStr = "Uploading";
                lblFileUploadStatus.Visibility = Visibility.Visible;
                lblFileUploadStatus.Content = connectionStr;
                DispatcherTimer fileStatusLblTimer = new DispatcherTimer();
                fileStatusLblTimer.Interval = TimeSpan.FromMilliseconds(250);
                int dotCount = 1;
                fileStatusLblTimer.Tick += (sender1, args) => {
                    lblFileUploadStatus.Content = connectionStr;
                    for (int i = 0; i < dotCount; i++)
                        lblFileUploadStatus.Content += ".";
                    dotCount = (dotCount + 1) % 4;
                };
                fileStatusLblTimer.Start();
                ftp.sendFile(filePath, ref fileStatusLblTimer);

                /*Task sendFile = Task.Factory.StartNew(() => { ftp.sendFile(filePath, ref fileStatusLblTimer); });
                Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action<Task>(async (task) => {
                    await Task.Run(() => {
                        task.Wait();
                    });
                    lblFileUploadStatus.Content = "Finished sending";
                }), sendFile);*/
              
            }
            //   ftp.Close();
            
        }

    }
}
