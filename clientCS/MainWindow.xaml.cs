using System.Windows;
using client;
using System.IO;
using System.Threading;
using System;
using Microsoft.Win32;
using System.Windows.Controls;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Diagnostics;
using System.Collections.Generic;
using System.Windows.Threading;
using System.Linq;
using System.Text;
using System.Configuration;


namespace clientCS {
    public class FilesViewItem {
        public string fileName { get; set; }
        public string fileSize { get; set; }
        public bool isChecked { get; set; }
        public int checkBoxId { get; set; }

        public string downloadProgress { get; set; }
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

            filesGridView.Columns[3].Width = 0;
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
            btnUpload.IsEnabled = false;
            btnUpload.Content = connectionStr;
            DispatcherTimer fileStatusLblTimer = new DispatcherTimer();
            fileStatusLblTimer.Interval = TimeSpan.FromMilliseconds(250);
            int dotCount = 1;
            fileStatusLblTimer.Tick += (sender1, args) => {
                btnUpload.Content = connectionStr;
                for (int i = 0; i < dotCount; i++)
                    btnUpload.Content += ".";
                dotCount = (dotCount + 1) % 4;
            };
            fileStatusLblTimer.Start();
            Task sendFile = Task.Factory.StartNew(() => { ftp.sendFile(filePath, ref fileStatusLblTimer); });
            Dispatcher.BeginInvoke(DispatcherPriority.Background ,new Action<Task>(async (task) => {
                await Task.Run(()=> {
                    task.Wait();
                });
                btnUpload.IsEnabled = true;
                btnUpload.Content = "Upload";
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

        public static List<int> AllIndexesOf(string str, string value) {
            if (String.IsNullOrEmpty(value))
                throw new ArgumentException("the string to find may not be empty", "value");
            List<int> indexes = new List<int>();
            for (int index = 0; ; index += value.Length) {
                index = str.IndexOf(value, index);
                if (index == -1)
                    return indexes;
                indexes.Add(index);
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
                                        List<FilesViewItem> filesList = new List<FilesViewItem>();
                                        // TODO: KEEP LISTENING FOR DATA WHILE YOU CAN (availiable)
                                        while (dataSoket.Available > 0) {
                                            dataSoket.Receive(buffer);

                                            string result = System.Text.Encoding.ASCII.GetString(buffer);
                                            string resultNoCode = result.Substring(result.IndexOf(':', result.IndexOf(':') + 1) + 1);
                                            string[] tuples = resultNoCode.Split(',');
                                            
                                            int id = 0;
                                            foreach (string s in tuples) {
                                                string[] filesColInfo = s.Split('^');
                                                if (filesColInfo.Length > 1) {
                                                    if (filesColInfo[0].Split(':').Length > 1)
                                                    {
                                                        string fileName = (filesColInfo[0].Split(':')[1]);
                                                        string fileSize = (filesColInfo[1].Split(':')[1]);

                                                        filesList.Add(new FilesViewItem { fileName = fileName, fileSize = fileSize, isChecked = false, checkBoxId = id });
                                                        id++;
                                                    }
                                                }
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
                
                txtFolderPath.Text = dlg.SelectedPath;
             
            }

        }

        private void btnSendFolder_Click(object sender, RoutedEventArgs e)
        {
            this.files = Directory.GetFiles(txtFolderPath.Text);
            Task sendFile = null;
            foreach (string filePath in files)
            {
                string connectionStr = "Uploading";
                btnUploadFolder.IsEnabled = false;
                btnUploadFolder.Content = connectionStr;
                DispatcherTimer fileStatusLblTimer = new DispatcherTimer();
                fileStatusLblTimer.Interval = TimeSpan.FromMilliseconds(250);
                int dotCount = 1;
                fileStatusLblTimer.Tick += (sender1, args) => {
                    btnUploadFolder.Content = connectionStr;
                    for (int i = 0; i < dotCount; i++)
                        btnUploadFolder.Content += ".";
                    dotCount = (dotCount + 1) % 4;
                };
                fileStatusLblTimer.Start();
                if (sendFile != null)
                    sendFile.Wait();
                sendFile = Task.Factory.StartNew(() => { ftp.sendFile(filePath, ref fileStatusLblTimer); });
                Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action<Task>(async (task) => {
                    await Task.Run(() => {
                        task.Wait();
                    });
                    btnUploadFolder.IsEnabled = true;
                    btnUploadFolder.Content = "Upload";
                }), sendFile);

                /*string connectionStr = "Uploading";
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

                Task sendFile = Task.Factory.StartNew(() => { ftp.sendFile(filePath, ref fileStatusLblTimer); });
                Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action<Task>(async (task) => {
                    await Task.Run(() => {
                        task.Wait();
                    });
                    lblFileUploadStatus.Content = "Finished sending";
                }), sendFile);*/
              
            }
        }

        public static int CountStringOccurrences(string text, string pattern) {
            // Loop through all instances of the string 'text'.
            int count = 0;
            int i = 0;
            while ((i = text.IndexOf(pattern, i)) != -1) {
                i += pattern.Length;
                count++;
            }
            return count;
        }

        string Convert(byte[] data) {
            char[] characters = data.Select(b => (char)b).ToArray();
            return new string(characters);
        }

        private void MyFiles_btnDownload_Click(object sender, RoutedEventArgs e) {
            var allItems = listView_Files.Items;

            var selectedItems = from FilesViewItem item in listView_Files.Items
                                where item.isChecked == true
                                select item;

            List<string> filesToDownload = new List<string>();
            foreach (var item in selectedItems) {
                string fileInfo = item.fileName;
                filesToDownload.Add(fileInfo);
            }

            if (filesToDownload.Count() > 0) {
                if (ftp != null) {
                    ftp.sendGetFiles(filesToDownload);
                }
            } else return;

            CancellationTokenSource cts = new CancellationTokenSource();
            cts.CancelAfter(20000);

            DispatcherTimer fileStatusLblTimer = new DispatcherTimer();
            fileStatusLblTimer.Interval = TimeSpan.FromMilliseconds(250);
            int dotCount = 1;
            fileStatusLblTimer.Tick += (sender1, args) => {
                myFiles_btnDownload.IsEnabled = false;
                myFiles_btnDownload.Content = "Downloading";
                for (int i = 0; i < dotCount; i++)
                    myFiles_btnDownload.Content += ".";
                dotCount = (dotCount + 1) % 4;
            };
            fileStatusLblTimer.Start();

            string file_end_str = "|pass:server_file_end";
            string file_start_str = "|pass:server_file_start:";
            Task downloadTask = Task.Factory.StartNew(() => {
                try {
                    Socket dataSoket = ftp.getDataSocket();
                    while (dataSoket.Available < 0) {
                        if (cts.IsCancellationRequested)
                            throw new Exception("Timeout");
                    }

                    int totalbytes = 0;
                    int bytesRecv = 0;
                    StringBuilder totalData = new StringBuilder();
                    MemoryStream ms = new MemoryStream();

                    do {
                        int bufferSize = 10 * 1024 * 1024;
                        byte[] buffer = new byte[bufferSize];

                        if ((bytesRecv = dataSoket.Receive(buffer)) > 0) {
                            ms.Write(buffer, 0, bytesRecv);
                            totalbytes += bytesRecv;
                            totalData.Append(System.Text.Encoding.UTF8.GetString(buffer, 0, buffer.Length));
                        }

                    } while (CountStringOccurrences(totalData.ToString(), file_end_str) < filesToDownload.Count());

                    byte[] totalByteData = ms.ToArray();

                    totalData.Clear();
                    ms.Close();

                    string testStr = Convert(totalByteData);
                    string[] totalFilesData = testStr.ToString().Split(new string[] { file_end_str }, StringSplitOptions.None);

                    testStr = String.Empty;

                    foreach (string fileData in totalFilesData) {
                        if (fileData.Length > 0) {
                            string dataWithName = fileData.Replace(file_start_str, "");
                            int nameEndPos = dataWithName.IndexOf(':');
                            string fileName = dataWithName.Substring(0, nameEndPos);
                            string dataNoName = dataWithName.Replace(fileName + ":", "");

                            dataWithName = String.Empty;

                            string downloadsFolder = ConfigurationManager.AppSettings.Get("Downloads Folder");
                            string filePath = Path.Combine("", downloadsFolder);
                            Directory.CreateDirectory(filePath);
                            filePath = Path.Combine(filePath, fileName);
                            using (FileStream fs = new FileStream(filePath, FileMode.Create)) {
                                fs.Write(dataNoName.Select(c => (byte)c).ToArray(), 0, dataNoName.Length);

                                fs.Flush();
                                fs.Close();
                            }
                        }
                    }
                } catch (Exception ex) {
                    Console.WriteLine(ex.Message);
                }

            });

            Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action<Task>(async (task) => {
                await Task.Run(() => {
                    task.Wait();
                });
                fileStatusLblTimer.Stop();
                myFiles_btnDownload.Content = "Download";
                myFiles_btnDownload.IsEnabled = true;
            }), downloadTask);
        }

        private void MyFiles_btnDownloadFolder_Click(object sender, RoutedEventArgs e) {
            string downloadFolder = ConfigurationManager.AppSettings.Get("Downloads Folder");
            string folderPath = Path.Combine("", downloadFolder);
            Process.Start(downloadFolder);
        }
    }
}
