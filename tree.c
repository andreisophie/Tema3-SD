#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "utils.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

FileTree *createFileTree(char* rootFolderName)
{
    FileTree *new_ft = malloc(sizeof(FileTree));
    DIE(!new_ft, "malloc failed createFileTree: new_ft\n");

    // alocare memorie folder
    TreeNode *root = emptyDir(rootFolderName);
    new_ft->root = root;

    return new_ft;
}

// functie care elibereaza memoria alocata
// pentru un fisier
void freeFile(FileContent *file)
{
    free(file->text);
    free(file);
}

// functie care elibereaza memoria alocata
// pentru un folder
void freeFolder(FolderContent *folder)
{
    ListNode *current = folder->children->head;
    ListNode *next_node;
    // eliminam memoria pentru fiecare folder/ fisier
    // din interiorul folderului care se sterge
    while (current) {
        next_node = current->next;
        freeNode(current->info);
        free(current);
        current = next_node;
    }
    free(folder->children);
    free(folder);
}

// functie care elibereaza memoria alocata unui nod
void freeNode(TreeNode *node)
{
    if (node->type == FILE_NODE)
        freeFile(node->content);
    else
        freeFolder(node->content);
    free(node->name);
    free(node);
}

// functie care elibereaza memoria alocata unui arbore
void freeTree(FileTree *fileTree)
{
    freeNode(fileTree->root);
    free(fileTree);
}

// functie care afiseaza folderele/ fisierele dintr-un
// anumit director
void ls_print_folder(TreeNode *currentNode)
{
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    while (node) {
        printf("%s\n", node->info->name);
        node = node->next;
    }
}

void ls(TreeNode* currentNode, char* arg)
{
    if (arg[0] == '\0') {
        // ls nu are argumente
        ls_print_folder(currentNode);
    } else {
        // se doreste afisarea unui anumit director/ fisier
        List *list = ((FolderContent *)currentNode->content)->children;
        ListNode *node = list->head;
        // se cauta directorul/ fisierul cu numele dat
        while (node) {
            if (strcmp(node->info->name, arg) == 0) {
                currentNode = node->info;
                break;
            }
            node = node->next;
        }
        if (!node) {
            // nu exista directorul/ fisierul cautat
            printf("ls: cannot access '%s': No such file or directory", arg);
        } else {
            // exista directorul/ fisierul cautat
            if (currentNode->type == FOLDER_NODE) {
                // afisam continutul directorului dorit
                ls_print_folder(currentNode);
            } else {
                // afisam continutul fisierului dorit
                printf("%s: %s\n", arg,
                ((FileContent *)currentNode->content)->text);
            }
        }
    }
}

void pwd(TreeNode* treeNode)
{
    // s-a ajuns in root
    if (!treeNode->parent) {
        printf("%s", treeNode->name);
        return;
    }

    // se afiseaza recursiv fiecare director din cale
    pwd(treeNode->parent);
    printf("/%s", treeNode->name);
}

// functie care cauta un anumit nod in directorul actual
TreeNode* search_node(TreeNode* currentNode, char* arg)
{
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    while (node) {
        if (strcmp(node->info->name, arg) == 0)
            return node->info;
        node = node->next;
    }
    if (!node)
        return NULL;
}

// functie care returneaza directorul cautat dintr-o cale data
TreeNode* searchDir(TreeNode* currentNode, char* path)
{
    char *pathCpy = strdup(path);
    TreeNode *node = currentNode;
    if (strcmp(pathCpy, "..") == 0) {
        // am ajuns in root
        if (currentNode->parent) {
            free(pathCpy);
            return currentNode->parent;
        } else {
            free(pathCpy);
            return NULL;
        }
    }
    // parsarea directoarelor pe care le parcurgem
    char *token = strtok(pathCpy, "/\n\0");
    while (token) {
        if (strcmp(token, "..") == 0) {
            node = node->parent;
        } else {
            node = search_node(node, token);
            if (!node || node->type == FILE_NODE) {
                // argument invalid, in cale trebuie sa
                // existe doar directoare
                free(pathCpy);
                return NULL;
            }
        }
        token = strtok(NULL, "/\0");
    }
    free(pathCpy);
    return node;
}

TreeNode* cd(TreeNode* currentNode, char* path)
{
    // directorul in care vreau sa ajung
    TreeNode *node = searchDir(currentNode, path);
    if (!node) {
        // cale invalida, nu se modifica directorul
        printf("cd: no such file or directory: %s", path);
        return currentNode;
    }
    // se modifica directorul curent
    return node;
}

// functie care afiseaza ierarhia de fisiere la apelarea comenzii "tree"
void printTree(TreeNode* currentNode, int tabs, int *dirs, int *files)
{
    for (int i = 0; i < tabs; i++)
        printf("\t");

    printf("%s\n", currentNode->name);
    if (currentNode->type == FILE_NODE) {
        // am gasit un nou fisier, ne intorcem in director
        // si continuam cautarea recursiva
        (*files)++;
        return;
    }

    // am gasit un nou director
    (*dirs)++;
    // parcurgem fisierele/ directoarele din acesta
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    while (node) {
        // am gasit un nou fisier/ director pe care il vom parcurge
        printTree(node->info, tabs + 1, dirs, files);
        node = node->next;
    }
}

void tree(TreeNode* currentNode, char* arg)
{
    // gasim directorul radacina pentru ierahie
    TreeNode *folder = searchDir(currentNode, arg);
    // s-a introdus un nume invalid de director
    if (!folder) {
        printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
        return;
    }

    // parcurgem ierarhia de fisiere si afisam in formatul cerut
    int dirs = 0, files = 0;
    List *list = ((FolderContent *)folder->content)->children;
    ListNode *node = list->head;
    while (node) {
        printTree(node->info, 0, &dirs, &files);
        node = node->next;
    }
    printf("%d directories, %d files", dirs, files);
}

// functie care aloca memorie pentru crearea unui director
TreeNode *emptyDir(char* folderName)
{
    TreeNode *newDir = malloc(sizeof(TreeNode));
    DIE(!newDir, "malloc failed createFileTree: root\n");

    newDir->parent = NULL;
    newDir->name = folderName;
    newDir->type = FOLDER_NODE;

    FolderContent *dirContent = malloc(sizeof(FolderContent));
    DIE(!dirContent, "malloc failed createFileTree: FolderContent\n");

    List* list = malloc(sizeof(List));
    DIE(!list, "malloc failed createFileTree: FolderContent\n");

    dirContent->children = list;
    list->head = NULL;
    newDir->content = dirContent;

    return newDir;
}

// functie care verifica daca un fisier exista intr-un director
int checkFile(TreeNode *currentNode, char* fileName)
{
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    while (node) {
        if (strcmp(node->info->name, fileName) == 0) {
            return 0;
        }
        node = node->next;
    }
    return 1;
}

void mkdir(TreeNode* currentNode, char* folderName)
{
    // exista deja un director cu numele introdus
    if (!checkFile(currentNode, folderName)) {
        printf("mkdir: cannot create directory '%s': File exists", folderName);
        free(folderName);
        return;
    }
    // se creeaza noul director
    TreeNode *newDir = emptyDir(folderName);
    newDir->parent = currentNode;
    addChild(currentNode, newDir);
}

void rmrec(TreeNode* currentNode, char* resourceName)
{
    // eliminam continutul directorul care va fi eliminat
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    ListNode *prev;
    while (node) {
        if (strcmp(node->info->name, resourceName) == 0) {
            if (node == list->head) {
                list->head = node->next;
                break;
            }
            prev->next = node->next;
            break;
        }
        prev = node;
        node = node->next;
    }
    // nu exista directorul care se doreste sa fie eliminat
    if (node == NULL) {
        printf("rmrec: failed to remove '%s': No such file or directory",
        resourceName);
        return;
    }
    freeNode(node->info);
    free(node);
}

void rm(TreeNode* currentNode, char* fileName)
{
    // cautam in director fisierul care trebuie eliminat
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    ListNode *prev;
    while (node) {
        if (strcmp(node->info->name, fileName) == 0) {
            if (node->info->type == FOLDER_NODE) {
                printf("rm: cannot remove '%s': Is a directory", fileName);
                return;
            }
            if (node == list->head) {
                list->head = node->next;
                break;
            }
            prev->next = node->next;
            break;
        }
        prev = node;
        node = node->next;
    }
    // fisierul cu numele dorit nu exista
    if (node == NULL) {
        printf("rm: failed to remove '%s': No such file or directory",
            fileName);
        return;
    }
    freeNode(node->info);
    free(node);
}

void rmdir(TreeNode* currentNode, char* folderName)
{
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    ListNode *prev;
    while (node) {
        if (strcmp(node->info->name, folderName) == 0) {
            if (node->info->type == FILE_NODE) {
                printf("rmdir: failed to remove '%s': Not a directory",
                    folderName);
                return;
            }
            if (((FolderContent *)node->info->content)->children->head
                != NULL) {
                printf("rmdir: failed to remove '%s': Directory not empty",
                    folderName);
                return;
            }
            if (node == list->head) {
                list->head = node->next;
                break;
            }
            prev->next = node->next;
            break;
        }
        prev = node;
        node = node->next;
    }
    if (node == NULL) {
        printf("rmdir: failed to remove '%s': No such file or directory",
            folderName);
        return;
    }
    freeNode(node->info);
    free(node);
}

void addChild(TreeNode* folder, TreeNode *treeNode)
{
    if (folder->type != FOLDER_NODE) {
        printf("error addLast: param is not folder\n");
        return;
    }
    List *list = ((FolderContent *)folder->content)->children;
    ListNode *newNode = malloc(sizeof(ListNode));
    DIE(!newNode, "malloc failed addLast: newNode\n");

    newNode->info = treeNode;
    newNode->next = list->head;
    list->head = newNode;
}

void touch(TreeNode* currentNode, char* fileName, char* fileText)
{
    if (!checkFile(currentNode, fileName)) {
        return;
    }

    TreeNode *newFile = malloc(sizeof(TreeNode));
    DIE(!newFile, "malloc failed touch: newFile\n");

    newFile->parent = currentNode;
    newFile->name = fileName;
    newFile->type = FILE_NODE;

    FileContent *fileContent = malloc(sizeof(FileContent));
    DIE(!fileContent, "malloc failed touch: fileContent\n");

    if (fileContent)
        fileContent->text = fileText;
    else
        fileContent->text = NULL;

    newFile->content = fileContent;
    addChild(currentNode, newFile);
}

TreeNode* searchFile(TreeNode* currentNode, char* path)
{
    char *pathCpy = strdup(path);
    TreeNode *node = currentNode;
    if (strcmp(pathCpy, "..") == 0) {
        free(pathCpy);
        return NULL;
    }
    char *token = strtok(pathCpy, "/\n\0");
    while (token) {
        if (strcmp(token, "..") == 0) {
            node = node->parent;
        } else {
            node = search_node(node, token);
            if (node && node->type == FILE_NODE) {
                free(pathCpy);
                return node;
            }
            if (!node) {
                free(pathCpy);
                return NULL;
            }
        }
        token = strtok(NULL, "/\0");
    }
}

void cp(TreeNode* currentNode, char* source, char* destination)
{
    TreeNode *fileCopy = searchFile(currentNode, source);
    if (fileCopy == NULL) {
        printf("cp: -r not specified; omitting directory '%s'", source);
        return;
    }

    TreeNode *folder = searchDir(currentNode, destination);
    TreeNode *fileDest = searchFile(currentNode, destination);
    if (fileDest == NULL) {
        if (folder == NULL) {
            printf("cp: failed to access '%s': Not a directory", destination);
            return;
        }
        TreeNode *destCheck = searchFile(folder, fileCopy->name);

        if (destCheck == NULL) {
            // copiez direct
            touch(folder, strdup(fileCopy->name), strdup(((FileContent *)fileCopy->content)->text));
            return;
        }

        // schimb content
        free(((FileContent *)destCheck->content)->text);
        ((FileContent *)destCheck->content)->text = strdup(((FileContent *)fileCopy->content)->text);
        return;
    }

    free(((FileContent *)fileDest->content)->text);
    ((FileContent *)fileDest->content)->text = strdup(((FileContent *)fileCopy->content)->text);
}

void mv(TreeNode* currentNode, char* source, char* destination)
{
}

