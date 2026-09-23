// For the BST assignment I created a system_log.txt file to test the BST's performance.
// It read about 169,000 log entries (see the sample output at the end of this file) and displayed the requested data.
// Still, if we look at the tree performance function, the depth came out as 15, neither GOOD nor BAD. If the tree were balanced, we would expect about 9 steps.
// The output may be long.
// I mostly tried to explain the code with comments.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Log
{
    char process[50]; // process
    int count;
    char lastEntry[50];
    char lastMessage[800];
    struct Log *left;
    struct Log *right;
};

// I first designed the insert function recursively, but because the txt file is large
// I ran into problems and had to convert it to an iterative version.
struct Log *insertLog(struct Log *root, const char process[], const char entry[], const char message[])
{

    struct Log *newRoot = (struct Log *)malloc(sizeof(struct Log));
    strcpy(newRoot->process, process);
    strcpy(newRoot->lastEntry, entry);
    strcpy(newRoot->lastMessage, message);
    newRoot->count = 1; // I set the count to 1 because the process is being added for the first time.
    newRoot->left = NULL;
    newRoot->right = NULL;

    if (root == NULL)
    {
        return newRoot;
    }

    struct Log *ptr = root, *parent = NULL;

    while (ptr != NULL) // Move through the tree until we find the correct position
    {
        parent = ptr; // assigned to save our current position.

        if (strcmp(process, ptr->process) == 0)
        {
            ptr->count++;
            strcpy(ptr->lastEntry, entry);
            strcpy(ptr->lastMessage, message);
            free(newRoot);
            return root;
        }

        else if (strcmp(process, ptr->process) < 0)
        {
            ptr = ptr->left;
        }
        else
        {
            ptr = ptr->right;
        }
    }

    if (strcmp(process, parent->process) < 0)
    {
        parent->left = newRoot;
    }

    else
    {
        parent->right = newRoot;
    }

    return root;
}

void searchLog(struct Log *root, const char process[])
{
    if (root == NULL)
    {
        printf("[X] The requested process was not found in the system logs.\n");
        return;
    }

    if (strcmp(process, root->process) == 0) // if what we are looking for is found, print it to the screen.
    {
        printf("[✓] The process you searched for was found!\n");
        printf("    Process     : %s\n", root->process);
        printf("    Total logs  : %d\n", root->count);
        printf("    Last entry  : %s\n", root->lastEntry);
        printf("    Last message: %s\n", root->lastMessage);
        return;
    }
    if (strcmp(process, root->process) < 0)
    {
        searchLog(root->left, process);
    }
    else
    {
        searchLog(root->right, process);
    }
}

struct Log *deleteLog(struct Log *root, const char process[])
{
    if (root == NULL) // means the tree is empty or the process was not found.
    {
        return NULL;
    }

    if (strcmp(process, root->process) < 0) // if it is smaller than the current process, it is on the left
    {
        root->left = deleteLog(root->left, process);
    }

    else if (strcmp(process, root->process) > 0) // if it is greater than the current process, it is on the right
    {
        root->right = deleteLog(root->right, process);
    }

    // now the deletion
    else
    {
        // If there is no left child - the right child takes its place.
        if (root->left == NULL)
        {
            struct Log *temp = root->right; // temporarily store the right child
            free(root);                     // free the current node from memory
            return temp;                    // new root: becomes the right child
        }

        if (root->right == NULL) // If there is no right child - the left child takes its place
        {
            struct Log *temp = root->left; // temporarily store the left child
            free(root);                    // free the current node from memory
            return temp;                   // new root: becomes the left child
        }

        // inorder method (Successor)
        struct Log *temp = root->right;

        while (temp->left != 0) // we find the smallest value by repeatedly going left in the right subtree
        {
            temp = temp->left;
        }

        // copy all data of the successor node into the current node
        // this way the successor takes the place of the node being deleted
        strcpy(root->process, temp->process);
        strcpy(root->lastEntry, temp->lastEntry);
        strcpy(root->lastMessage, temp->lastMessage);
        root->count = temp->count;

        // now the original successor node is deleted from the right subtree
        // because we copied its data up, there cannot be two of them
        root->right = deleteLog(root->right, temp->process);

        return root;
    }

    return root; // if we went left or right, returns the current root as it is
}

// Since our txt file is very messy, I started with a function that reads the file correctly.
void readLogFile(struct Log **root, const char fileName[])
{
    FILE *file = fopen(fileName, "r"); // we open the file in read ("r") mode.
    if (file == NULL)
    {
        printf("File not found!\n");
        return;
    }

    printf(">>> File opened\n");

    char line[800];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) // Read line by line until the end of the file
    {
        char date[20];
        char time[25];
        char thread[20]; // unit of execution
        char type[20];
        char activity[20];

        int pid;        // process id
        int ttl;        // how long will the log be kept?
        char rest[800]; // buffer, e.g. the rest variable holds "kernel: (com.apple.DriverKit...) checkTraffic..."

        // we skip the spaces thanks to sscanf.
        int fields = sscanf(line, "%14s %24s %14s %19s %9s %d %d %799[^\n]", date, time, thread, type, activity, &pid, &ttl, rest);

        if (fields < 8) // if not all 8 fields were read, the line is skipped.
        {
            continue;
        }

        char process[50];
        char *colon = strchr(rest, ':');

        if (colon == NULL) // If there is no ":", this line has no process info, skip it
        {
            continue;
        }

        int length = colon - rest;
        if (length >= 50)
        {
            length = 49;
        }

        strncpy(process, rest, length);
        process[length] = '\0';

        if (process[0] == '=' || strchr(process, ' ') != NULL) // skip process names that start with = or contain a space
            continue;

        *root = insertLog(*root, process, time, rest); // insert the process name into the BST
    }

    fclose(file); // we close the file now.
}

void listLogs(struct Log *root) // Lists all logs in the existing txt file
                                // like listing words.
{
    if (root == NULL)
        return;

    listLogs(root->left);                       // left first
    printf("%-30s %5d log    Last Entry: %s\n", // then itself
           root->process, root->count, root->lastEntry);
    listLogs(root->right); // then right
}

void Statistics(struct Log *root, int *totalLogs, int *unique, struct Log **mostActive, struct Log **leastActive)
{
    if (root == NULL) // return if the root is empty
    {
        return;
    }

    (*unique)++;
    *totalLogs += root->count;

    // most active log check
    if (*mostActive == NULL || root->count > (*mostActive)->count)
        *mostActive = root;

    // quietest log check
    if (*leastActive == NULL || root->count < (*leastActive)->count)
        *leastActive = root;

    Statistics(root->left, totalLogs, unique, mostActive, leastActive);
    Statistics(root->right, totalLogs, unique, mostActive, leastActive);
}

void logStatistics(struct Log *root)
{
    if (root == NULL)
    {
        printf("Statistics could not be generated. The list is empty!\n");
        return;
    }

    int totalLogs = 0; // our total log count is 0 at this point
    int unique = 0;    // unique is also 0 at this point
    struct Log *mostActive = NULL;
    struct Log *leastActive = NULL;

    Statistics(root, &totalLogs, &unique, &mostActive, &leastActive);

    float average = (float)totalLogs / unique;

    printf("\n===== LOG STATISTICS =====\n");
    printf("Total number of logs       : %d\n", totalLogs);
    printf("Unique processes           : %d\n", unique);
    printf("Most active process        : %s (%d logs)\n", mostActive->process, mostActive->count);
    printf("Quietest process           : %s (%d logs)\n", leastActive->process, leastActive->count);
    printf("Average logs per process   : %.2f\n", average);
}

int treeDepth(struct Log *treeRoot) // function that prints the depth of the tree I built (its performance)
                                    // and its theoretical minimum.
{
    if (treeRoot == NULL)
        return 0;

    int left = treeDepth(treeRoot->left);
    int right = treeDepth(treeRoot->right);

    return 1 + (left > right ? left : right); // I got help from AI for this function (treeDepth); the idea was mine,
                                              // but I struggled to build the function even though it is short.
}

int main()
{
    struct Log *root = NULL;

    readLogFile(&root, "system_log.txt");

    // === MANUAL LOG INSERTION ===
    printf("\n=== Adding new log: dataStructures ===\n");
    root = insertLog(root, "dataStructures", "17:00:00", "dataStructures: application started");
    printf("Added.\n");

    printf("\n=== Adding again now: dataStructures ===\n");
    root = insertLog(root, "dataStructures", "17:05:00", "dataStructures: file saved");
    printf("Added.\n");

    struct Log *wordRoot = NULL;

    printf("\n===== TREE ANALYSIS =====\n");
    printf("Tree depth       : %d\n", treeDepth(root));
    printf("Theoretical min  : log2(N) = ~9\n");

    // === SEARCH ===
    printf("\n=== Search: myApp ===\n");
    searchLog(root, "myApp");

    printf("\n=== Search: kernel ===\n");
    searchLog(root, "kernel");

    printf("\n=== Search: bluetoothd ===\n");
    searchLog(root, "bluetoothd");

    printf("\n=== Search: xyzsjhbs ===\n");
    searchLog(root, "xyzsjhbs");

    printf("\n=== Search: dataStructures ===\n");
    searchLog(root, "dataStructures");

    printf("\n===== ALL PROCESSES =====\n"); // here it lists all logs
                                             // found in the txt file.
                                             // can be thought of like a list of all words.
    listLogs(root);

    logStatistics(root);

    printf("\n=== Delete: kernel ===\n");
    root = deleteLog(root, "kernel");
    printf("\n=== Search after deletion: kernel ===\n");
    searchLog(root, "kernel"); // search again, it should be deleted now.

    printf(">>> File closed\n");

    return 0;
}

// THE PROGRAM OUTPUT IS SHOWN BELOW.

/*

>>> File opened

=== Adding new log: dataStructures ===
Added.

=== Adding again now: dataStructures ===
Added.

===== TREE ANALYSIS =====
Tree depth       : 15
Theoretical min  : log2(N) = ~9

=== Search: myApp ===
[X] The requested process was not found in the system logs.

=== Search: kernel ===
[✓] The process you searched for was found!
    Process     : kernel
    Total logs  : 11113
    Last entry  : 16:17:48.331272+0300
    Last message: kernel: (com.apple.DriverKit-AppleBCMWLAN.dext) wlan0:com.apple.p2p.awdl0: currentInfraTrafficType:9270 checking if realtime upgrade required with inputPackets:2 outputPackets:0 packetThreshold:50

=== Search: bluetoothd ===
[✓] The process you searched for was found!
    Process     : bluetoothd
    Total logs  : 10788
    Last entry  : 16:17:48.444010+0300
    Last message: bluetoothd: [com.apple.bluetooth:Server.LE.Scan] decryptProximityPairingPayload device:<private> address:<private>

=== Search: xyzsjhbs ===
[X] The requested process was not found in the system logs.

=== Search: dataStructures ===
[✓] The process you searched for was found!
    Process     : dataStructures
    Total logs  : 2
    Last entry  : 17:05:00
    Last message: dataStructures: file saved

===== ALL PROCESSES =====
                                 154 log    Last Entry: of
AccessibilityUIServer             11 log    Last Entry: 16:10:51.362059+0300
AddressBookManager               340 log    Last Entry: 16:15:26.823039+0300
AddressBookSourceSync           2300 log    Last Entry: 16:15:52.087685+0300
AppPredictionIntentsHelperService    20 log    Last Entry: 16:17:08.257067+0300
AppSSODaemon                       2 log    Last Entry: 16:09:58.920360+0300
AppleIDSettings                 1350 log    Last Entry: 16:08:19.971716+0300
AppleSpell                       274 log    Last Entry: 16:17:37.946710+0300
AssetCacheLocatorService           1 log    Last Entry: 16:10:12.051951+0300
AuthenticationServicesAgent       16 log    Last Entry: 16:08:58.852980+0300
BackgroundShortcutRunner         270 log    Last Entry: 16:17:08.251664+0300
BatteriesAvocadoWidgetExtension   337 log    Last Entry: 16:17:11.073633+0300
BiomeAgent                      1186 log    Last Entry: 16:17:43.236678+0300
CGPDFService                      48 log    Last Entry: 16:14:17.790827+0300
ChatGPT                         1081 log    Last Entry: 16:17:25.628432+0300
ChatGPTHelper                    497 log    Last Entry: 16:16:57.527170+0300
ClassroomSettings                 87 log    Last Entry: 16:08:19.971621+0300
Claude                           343 log    Last Entry: 16:17:11.609208+0300
CloudTelemetryService             17 log    Last Entry: 16:17:30.312134+0300
Code                             153 log    Last Entry: 16:16:32.972639+0300
CommCenter                        61 log    Last Entry: 16:15:33.155364+0300
ContainerMetadataExtractor         6 log    Last Entry: 16:17:08.087587+0300
ContextStoreAgent                359 log    Last Entry: 16:17:43.225218+0300
ContinuityCaptureAgent          1044 log    Last Entry: 16:17:40.975712+0300
ControlCenter                   1876 log    Last Entry: 16:17:43.322321+0300
CoreServicesUIAgent               72 log    Last Entry: 16:15:06.561345+0300
CursorUIViewService              902 log    Last Entry: 16:17:43.330192+0300
DefaultExtensionEnablement       458 log    Last Entry: 16:08:19.981331+0300
Dock                             223 log    Last Entry: 16:17:43.222394+0300
DockHelper                        22 log    Last Entry: 16:08:32.100482+0300
FSKitModuleManagement             97 log    Last Entry: 16:08:19.983792+0300
FamilySettings                   326 log    Last Entry: 16:08:19.978736+0300
FeatureAccessAgent               264 log    Last Entry: 16:13:12.188127+0300
Finder                           103 log    Last Entry: 16:16:32.972798+0300
FollowUpSettingsExtension        110 log    Last Entry: 16:08:19.979319+0300
Gemini                          4929 log    Last Entry: 16:17:43.322326+0300
GeneralSettings                  189 log    Last Entry: 16:08:19.971226+0300
GroupSessionService               59 log    Last Entry: 16:13:30.229416+0300
HeadphoneSettingsExtension       148 log    Last Entry: 16:08:19.971638+0300
IMDPersistenceAgent               82 log    Last Entry: 16:16:18.010440+0300
IntelligencePlatformComputeService   190 log    Last Entry: 16:17:43.226677+0300
LegacyPluginEnablement           103 log    Last Entry: 16:08:19.984253+0300
LoginItems                       624 log    Last Entry: 16:08:19.978912+0300
MSTeams                          220 log    Last Entry: 16:16:36.038106+0300
MTLCompilerService               839 log    Last Entry: 16:14:41.453705+0300
MediaExtensionsSettingsController    92 log    Last Entry: 16:08:19.981086+0300
Notes                            413 log    Last Entry: 16:16:04.155750+0300
NotificationCenter              1475 log    Last Entry: 16:17:14.401400+0300
PAH_Extension                    354 log    Last Entry: 16:17:43.468379+0300
PerfPowerServices                835 log    Last Entry: 16:17:48.028983+0300
PowerUIAgent                      15 log    Last Entry: 16:17:14.391883+0300
Preview                          399 log    Last Entry: 16:13:01.371595+0300
ProtectedCloudKeySyncing           2 log    Last Entry: 16:08:13.881942+0300
QuickLookUIHelper                225 log    Last Entry: 16:14:23.299097+0300
QuickLookUIService                65 log    Last Entry: 16:08:55.398730+0300
Safari                           407 log    Last Entry: 16:15:40.135776+0300
SafariBookmarksSyncAgent          96 log    Last Entry: 16:15:40.117164+0300
ScopedBookmarkAgent               20 log    Last Entry: 16:08:36.032831+0300
ScreenTimeAgent                    6 log    Last Entry: 16:10:20.114367+0300
SetStoreUpdateService             34 log    Last Entry: 16:15:36.640009+0300
SettingsSystemExtensionController   109 log    Last Entry: 16:08:19.981195+0300
ShortcutsViewService              12 log    Last Entry: 16:17:08.252563+0300
SidecarRelay                      37 log    Last Entry: 16:17:40.972146+0300
SoftwareUpdateNotificationManager    16 log    Last Entry: 16:09:29.699968+0300
Spotify                           42 log    Last Entry: 16:17:08.245279+0300
Spotlight                        328 log    Last Entry: 16:17:43.239877+0300
StatusKitAgent                     9 log    Last Entry: 16:09:31.710888+0300
Terminal                         473 log    Last Entry: 16:17:48.021190+0300
TextInputSwitcher                 50 log    Last Entry: 16:09:20.001789+0300
TextThumbnailExtension            20 log    Last Entry: 16:14:31.132168+0300
TrustedPeersHelper                 2 log    Last Entry: 16:08:14.044187+0300
UIKitSystem                      329 log    Last Entry: 16:17:11.272901+0300
UniversalControl                   7 log    Last Entry: 16:08:55.398810+0300
UsageTrackingAgent               226 log    Last Entry: 16:16:06.893248+0300
UserEventAgent                   457 log    Last Entry: 16:17:12.093560+0300
UserNotificationCenter            64 log    Last Entry: 16:14:15.958502+0300
VPN                              112 log    Last Entry: 16:08:19.978539+0300
VTDecoderXPCService               45 log    Last Entry: 16:13:16.814363+0300
ViewBridgeAuxiliary              303 log    Last Entry: 16:17:43.254840+0300
WallpaperAerialsExtension        406 log    Last Entry: 16:17:42.868667+0300
WeatherWidget                    418 log    Last Entry: 16:08:39.929277+0300
WiFiAgent                         41 log    Last Entry: 16:17:13.039756+0300
WindowManager                    491 log    Last Entry: 16:17:43.221600+0300
WindowServer                    7779 log    Last Entry: 16:17:48.020162+0300
WirelessRadioManagerd              2 log    Last Entry: 16:14:52.477539+0300
XPCKeychainSandboxCheck           18 log    Last Entry: 16:08:44.048813+0300
XprotectService                   14 log    Last Entry: 16:08:55.391940+0300
accessoryupdaterd                  3 log    Last Entry: 16:08:19.978500+0300
accountsd                       1341 log    Last Entry: 16:17:33.863879+0300
adid                              67 log    Last Entry: 16:17:29.884153+0300
adprivacyd                         5 log    Last Entry: 16:08:14.071723+0300
airportd                        4574 log    Last Entry: 16:17:47.959152+0300
akd                              990 log    Last Entry: 16:17:34.954609+0300
amsaccountsd                       9 log    Last Entry: 16:09:58.911699+0300
amsengagementd                    19 log    Last Entry: 16:15:38.157264+0300
analyticsd                        91 log    Last Entry: 16:17:08.251530+0300
aned                               1 log    Last Entry: 16:10:52.366912+0300
appleeventsd                     210 log    Last Entry: 16:14:27.279318+0300
appstoreagent                      2 log    Last Entry: 16:09:40.787177+0300
apsd                             576 log    Last Entry: 16:17:05.836084+0300
assessmentagent                   13 log    Last Entry: 16:10:42.376496+0300
assistantd                        95 log    Last Entry: 16:16:18.010340+0300
audioaccessoryd                 1252 log    Last Entry: 16:17:46.764749+0300
audioanalyticsd                   62 log    Last Entry: 16:17:39.773681+0300
audioclocksyncd                  160 log    Last Entry: 16:16:52.616287+0300
audiomxd                         149 log    Last Entry: 16:17:11.698581+0300
authd                             11 log    Last Entry: 16:09:40.784055+0300
avconferenced                      2 log    Last Entry: 16:10:29.185234+0300
axassetsd                          2 log    Last Entry: 16:10:29.186123+0300
backgroundtaskmanagementd         14 log    Last Entry: 16:10:27.159288+0300
backupd                          116 log    Last Entry: 16:17:36.071236+0300
betaenrollmentagent                6 log    Last Entry: 16:08:19.997637+0300
biomed                           239 log    Last Entry: 16:17:00.136714+0300
biomesyncd                       280 log    Last Entry: 16:17:03.270371+0300
bird                              62 log    Last Entry: 16:17:30.303968+0300
bluetoothd                     10788 log    Last Entry: 16:17:48.444010+0300
bluetoothuserd                    20 log    Last Entry: 16:08:58.944680+0300
calaccessd                       614 log    Last Entry: 16:16:28.845848+0300
callservicesd                     94 log    Last Entry: 16:16:18.010488+0300
cameracaptured                     4 log    Last Entry: 16:10:47.333861+0300
cdpd                             119 log    Last Entry: 16:08:58.823434+0300
cfprefsd                         479 log    Last Entry: 16:17:39.902029+0300
chronod                          923 log    Last Entry: 16:17:16.329860+0300
cloudd                          1172 log    Last Entry: 16:17:33.496251+0300
cloudphotod                       19 log    Last Entry: 16:08:14.639702+0300
colorsync.useragent                4 log    Last Entry: 16:10:56.392775+0300
com.apple.BKAgentService           8 log    Last Entry: 16:11:20.874817+0300
com.apple.CloudPhotosConfiguration    25 log    Last Entry: 16:15:33.048486+0300
com.apple.CodeSigningHelper       84 log    Last Entry: 16:14:23.284348+0300
com.apple.Safari.History           8 log    Last Entry: 16:08:14.099971+0300
com.apple.Virtualization.VirtualMachine    27 log    Last Entry: 16:14:09.243312+0300
com.apple.WebKit.GPU               5 log    Last Entry: 16:11:28.657384+0300
com.apple.WebKit.Networking      425 log    Last Entry: 16:17:11.609243+0300
com.apple.appkit.xpc.openAndSavePanelService   966 log    Last Entry: 16:16:32.972970+0300
com.apple.audio.Core-Audio-Driver-Service.helper     3 log    Last Entry: 16:10:41.415282+0300
com.apple.cmio.registerassistantservice   132 log    Last Entry: 16:17:40.974979+0300
com.apple.dock.extra              25 log    Last Entry: 16:15:44.375976+0300
com.apple.fskit.msdos              3 log    Last Entry: 16:10:38.256431+0300
com.apple.geod                    16 log    Last Entry: 16:13:39.032530+0300
com.apple.hiservices-xpcservice    30 log    Last Entry: 16:14:27.279181+0300
com.apple.iCloudHelper           637 log    Last Entry: 16:15:37.490402+0300
com.apple.quicklook.ThumbnailsAgent   458 log    Last Entry: 16:14:36.361001+0300
com.microsoft.teams2.teamsswitcher    14 log    Last Entry: 16:16:36.038156+0300
commerce                          26 log    Last Entry: 16:16:18.255610+0300
communicationtrustd              101 log    Last Entry: 16:16:18.010325+0300
configd                           14 log    Last Entry: 16:10:20.227836+0300
contactsd                       2019 log    Last Entry: 16:16:18.010628+0300
containermanagerd                 98 log    Last Entry: 16:17:15.304892+0300
contentlinkingd                   85 log    Last Entry: 16:12:37.098957+0300
contextstored                    383 log    Last Entry: 16:17:43.224888+0300
coreaudiod                       137 log    Last Entry: 16:17:43.214878+0300
coreauthd                         30 log    Last Entry: 16:15:40.949767+0300
corebrightnessd                 2374 log    Last Entry: 16:17:14.393490+0300
coreduetd                        226 log    Last Entry: 16:17:11.435986+0300
corespeechd                       66 log    Last Entry: 16:17:14.390896+0300
corespeechd_system                19 log    Last Entry: 16:10:41.424587+0300
corespotlightd                   952 log    Last Entry: 16:17:48.021841+0300
coresymbolicationd                 2 log    Last Entry: 16:09:22.226889+0300
countryd                          22 log    Last Entry: 16:15:00.508683+0300
ctkd                              69 log    Last Entry: 16:17:34.954709+0300
dasd                            1388 log    Last Entry: 16:17:45.415521+0300
dataStructures                     2 log    Last Entry: 17:05:00
dataaccessd                     4438 log    Last Entry: 16:16:10.768298+0300
diagnostics_agent                 19 log    Last Entry: 16:14:23.262911+0300
diskarbitrationd                   1 log    Last Entry: 16:10:38.248677+0300
distnoted                       1564 log    Last Entry: 16:17:08.252027+0300
dmd                                2 log    Last Entry: 16:10:20.114521+0300
donotdisturbd                     79 log    Last Entry: 16:16:18.009809+0300
duetexpertd                     3745 log    Last Entry: 16:17:43.225471+0300
eligibilityd                      10 log    Last Entry: 16:15:00.503908+0300
extensionkitservice               87 log    Last Entry: 16:17:11.047295+0300
familycircled                    138 log    Last Entry: 16:16:18.005743+0300
filecoordinationd                523 log    Last Entry: 16:17:08.252154+0300
fileproviderd                     79 log    Last Entry: 16:14:31.125842+0300
financed                           6 log    Last Entry: 16:08:14.122011+0300
findmybeaconingd                  29 log    Last Entry: 16:15:00.011907+0300
findmydeviced                     15 log    Last Entry: 16:11:20.876161+0300
findmylocateagent                 54 log    Last Entry: 16:15:33.273542+0300
followupd                          8 log    Last Entry: 16:08:19.998574+0300
fontd                             20 log    Last Entry: 16:12:38.640223+0300
fseventsd                        124 log    Last Entry: 16:17:37.088426+0300
fskit_agent                        2 log    Last Entry: 16:10:38.255922+0300
fskitd                             1 log    Last Entry: 16:09:40.784680+0300
gamed                            247 log    Last Entry: 16:16:14.565070+0300
gamepolicyd                     1709 log    Last Entry: 16:17:43.815113+0300
git                               16 log    Last Entry: 16:17:20.696723+0300
heard                           1430 log    Last Entry: 16:17:12.206481+0300
historicalaudiod                  50 log    Last Entry: 16:10:41.587855+0300
homed                            328 log    Last Entry: 16:16:32.972936+0300
homeenergyd                       11 log    Last Entry: 16:08:14.121564+0300
iCloudNotificationAgent           10 log    Last Entry: 16:15:33.052618+0300
iconservicesagent                102 log    Last Entry: 16:14:14.485206+0300
iconservicesd                     12 log    Last Entry: 16:09:59.928136+0300
identityservicesd                380 log    Last Entry: 16:17:42.463245+0300
imagent                         3687 log    Last Entry: 16:17:48.366444+0300
imklaunchagent                    12 log    Last Entry: 16:09:59.926865+0300
inputanalyticsd                   16 log    Last Entry: 16:17:39.867280+0300
intelligencecontextd             270 log    Last Entry: 16:17:00.718713+0300
intelligentroutingd               21 log    Last Entry: 16:17:11.698600+0300
itunescloudd                      27 log    Last Entry: 16:17:14.392453+0300
kernel                         11113 log    Last Entry: 16:17:48.331272+0300
keybagd                           16 log    Last Entry: 16:15:21.786073+0300
keyboardservicesd                 19 log    Last Entry: 16:17:37.861907+0300
knowledge-agent                  164 log    Last Entry: 16:16:18.010368+0300
launchd                         3310 log    Last Entry: 16:17:45.360107+0300
launchservicesd                  302 log    Last Entry: 16:17:43.214225+0300
linkd                            330 log    Last Entry: 16:17:40.151584+0300
liquiddetectiond                  48 log    Last Entry: 16:15:21.813511+0300
liveactivitiesd                    1 log    Last Entry: 16:09:31.708839+0300
locationd                       8786 log    Last Entry: 16:17:48.444594+0300
lockoutagent                       2 log    Last Entry: 16:08:11.718201+0300
log                                8 log    Last Entry: 16:17:47.926436+0300
login                             13 log    Last Entry: 16:17:00.737054+0300
logind                            16 log    Last Entry: 16:17:03.058655+0300
loginwindow                      543 log    Last Entry: 16:17:03.059891+0300
lsd                              232 log    Last Entry: 16:17:39.941021+0300
mDNSResponder                   3083 log    Last Entry: 16:17:46.877727+0300
maild                             24 log    Last Entry: 16:16:18.055688+0300
managedappdistributionagent        5 log    Last Entry: 16:10:19.113235+0300
managedcorespotlightd              1 log    Last Entry: 16:09:19.616343+0300
mapssyncd                          8 log    Last Entry: 16:08:14.120205+0300
mds                              503 log    Last Entry: 16:17:39.871773+0300
mds_stores                       242 log    Last Entry: 16:17:43.224898+0300
mdworker_shared                    3 log    Last Entry: 16:08:12.163910+0300
mediaanalysisd                     8 log    Last Entry: 16:15:33.049578+0300
mediaremoted                      28 log    Last Entry: 16:17:11.703361+0300
mlhostd                            1 log    Last Entry: 16:09:23.645994+0300
mobileactivationd                  3 log    Last Entry: 16:08:12.099316+0300
mobileassetd                      42 log    Last Entry: 16:17:46.969698+0300
naturallanguaged                   8 log    Last Entry: 16:13:31.266458+0300
neagent                            2 log    Last Entry: 16:17:29.742832+0300
nearbyd                           15 log    Last Entry: 16:17:07.038783+0300
nehelper                          30 log    Last Entry: 16:17:29.743204+0300
nesessionmanager                   5 log    Last Entry: 16:08:19.997937+0300
networkserviceproxy                6 log    Last Entry: 16:14:58.282091+0300
nsurlsessiond                    309 log    Last Entry: 16:15:26.977006+0300
opendirectoryd                    80 log    Last Entry: 16:17:29.956834+0300
osanalyticshelper                  2 log    Last Entry: 16:10:33.210146+0300
parsecd                           44 log    Last Entry: 16:13:58.450895+0300
passd                             28 log    Last Entry: 16:10:14.064517+0300
pboard                           122 log    Last Entry: 16:17:40.508765+0300
pbs                               14 log    Last Entry: 16:13:20.165207+0300
peopled                           72 log    Last Entry: 16:16:18.010280+0300
photolibraryd                     44 log    Last Entry: 16:15:33.056434+0300
pkd                              275 log    Last Entry: 16:17:39.066267+0300
postersyncd                      159 log    Last Entry: 16:16:18.010323+0300
powerd                          2768 log    Last Entry: 16:17:47.908136+0300
powerdatad                        18 log    Last Entry: 16:10:53.652477+0300
powerexperienced                  70 log    Last Entry: 16:17:14.390633+0300
proactived                       184 log    Last Entry: 16:17:43.230193+0300
progressd                          9 log    Last Entry: 16:08:58.952503+0300
rapportd                         391 log    Last Entry: 16:17:40.969446+0300
recentsd                          26 log    Last Entry: 16:16:18.060273+0300
remoted                            6 log    Last Entry: 16:09:51.862152+0300
routined                         137 log    Last Entry: 16:17:02.428585+0300
rtcreportingd                    192 log    Last Entry: 16:17:39.774408+0300
runningboardd                  15878 log    Last Entry: 16:17:43.814431+0300
sandboxd                          52 log    Last Entry: 16:16:59.733952+0300
scutil                             4 log    Last Entry: 16:17:00.836151+0300
searchpartyd                    3427 log    Last Entry: 16:17:48.445213+0300
searchpartyuseragent             190 log    Last Entry: 16:17:11.700921+0300
secd                             245 log    Last Entry: 16:15:32.973118+0300
secinitd                         185 log    Last Entry: 16:14:23.231528+0300
securityd                          7 log    Last Entry: 16:13:06.516124+0300
securityd_system                  20 log    Last Entry: 16:11:26.002400+0300
seld                              12 log    Last Entry: 16:10:13.081422+0300
sharedfilelistd                    9 log    Last Entry: 16:10:28.168547+0300
sharingd                         834 log    Last Entry: 16:17:42.463407+0300
siriactionsd                      86 log    Last Entry: 16:17:08.251997+0300
siriknowledged                    94 log    Last Entry: 16:16:18.010506+0300
sociallayerd                      42 log    Last Entry: 16:15:40.152661+0300
spindump                          77 log    Last Entry: 16:17:46.975279+0300
spotlightknowledged                9 log    Last Entry: 16:15:06.561494+0300
spotlightknowledged.updater      486 log    Last Entry: 16:17:48.021958+0300
studentd                         475 log    Last Entry: 16:16:18.005746+0300
suggestd                         896 log    Last Entry: 16:17:43.217784+0300
sw_vers                            3 log    Last Entry: 16:17:00.769740+0300
symptomsd                        469 log    Last Entry: 16:17:45.849040+0300
syncdefaultsd                     17 log    Last Entry: 16:08:14.713429+0300
sysextd                           84 log    Last Entry: 16:08:19.999545+0300
syspolicyd                     10186 log    Last Entry: 16:17:47.576761+0300
systemsettingsagent              100 log    Last Entry: 16:08:15.184659+0300
systemstats                       19 log    Last Entry: 16:17:12.759080+0300
talagentd                         43 log    Last Entry: 16:17:34.834049+0300
tccd                            4632 log    Last Entry: 16:17:37.862102+0300
thermalmonitord                    6 log    Last Entry: 16:15:05.125444+0300
timed                             19 log    Last Entry: 16:17:02.936258+0300
transparencyd                     93 log    Last Entry: 16:16:18.010374+0300
triald                             3 log    Last Entry: 16:09:51.886884+0300
trustd                          2707 log    Last Entry: 16:17:43.219382+0300
universalaccessd                   1 log    Last Entry: 16:08:13.850381+0300
useractivityd                    490 log    Last Entry: 16:17:40.511348+0300
usermanagerd                       5 log    Last Entry: 16:15:22.969831+0300
usernoted                         80 log    Last Entry: 16:16:18.005713+0300
usernotificationsd                 5 log    Last Entry: 16:10:03.982788+0300
voicebankingd                     29 log    Last Entry: 16:14:15.869925+0300
watchdogd                        377 log    Last Entry: 16:17:47.804415+0300
wifip2pd                          76 log    Last Entry: 16:17:29.160044+0300
xpcproxy                         225 log    Last Entry: 16:17:39.886927+0300
xprotectd                          3 log    Last Entry: 16:17:00.741750+0300
zsh                                1 log    Last Entry: 16:17:00.742377+0300

===== LOG STATISTICS =====
Total number of logs       : 169350
Unique processes           : 301
Most active process        : runningboardd (15878 logs)
Quietest process           : AssetCacheLocatorService (1 logs)
Average logs per process   : 562.62

=== Delete: kernel ===

=== Search after deletion: kernel ===
[X] The requested process was not found in the system logs.
>>> File closed

*/
