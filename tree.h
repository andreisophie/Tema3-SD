#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;
typedef struct ListNode ListNode;
typedef struct List List;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    List* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

struct ListNode {
    TreeNode* info;
    ListNode* next;
};

struct List {
    ListNode* head;
};

// *** FUNCTII APELATE DE COMENZI ***
void touch(TreeNode* currentNode, char* fileName, char* fileContent);

void ls(TreeNode* currentNode, char* arg);

void mkdir(TreeNode* currentNode, char* folderName);

TreeNode* cd(TreeNode* currentNode, char* path);

void tree(TreeNode* currentNode, char* arg);

void pwd(TreeNode* treeNode);

void rmdir(TreeNode* currentNode, char* folderName);

void rm(TreeNode* currentNode, char* fileName);

void rmrec(TreeNode* currentNode, char* resourceName);

void cp(TreeNode* currentNode, char* source, char* destination);

void mv(TreeNode* currentNode, char* source, char* destination);

// *** FUNCTII CARE CAUTA CAI ***
// functie care cauta un anumit nod in directorul actual
TreeNode* search_node(TreeNode* currentNode, char* arg);

// functie care verifica daca un fisier exista intr-un director
int checkFile(TreeNode *currentNode, char* fileName);

// functie care returneaza directorul cautat dintr-o cale data
TreeNode* searchDir(TreeNode* currentNode, char* path);

// functie care returneaza fisierul de la calea data, daca exista
TreeNode* searchFile(TreeNode* currentNode, char* path);

// *** FUNCTII DE AFISARE ***
// functie care afiseaza folderele/ fisierele dintr-un
// anumit director
void ls_print_folder(TreeNode *currentNode);

// functie care afiseaza ierarhia de fisiere la apelarea comenzii "tree"
void printTree(TreeNode* currentNode, int tabs, int *dirs, int *files);

// functie care initializeaza ierarhia de fisiere
FileTree *createFileTree(char* rootFolderName);

// functie care aloca memorie pentru crearea unui director
TreeNode *emptyDir(char* folderName);

// functie care adauga un copil unui folder
void addChild(TreeNode* folder, TreeNode *treeNode);

// *toate functiile elibereaza recursiv si copiii nodului

// functie care elibereaza memoria alocata
// pentru un fisier
void freeFile(FileContent *file);

// functie care elibereaza memoria alocata
// pentru un folder
void freeFolder(FolderContent *folder);

// functie care elibereaza memoria alocata unui nod
void freeNode(TreeNode *node);

// functie care elibereaza memoria alocata unui arbore
void freeTree(FileTree *fileTree);
