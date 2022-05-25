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
    TreeNode *root = emptyDir(rootFolderName);
    new_ft->root = root;

    return new_ft;
}

void freeFile(FileContent *file)
{
    free(file->text);
    free(file);
}

void freeFolder(FolderContent *folder)
{
    ListNode *current = folder->children->head;
    ListNode *next_node;
    while (current) {
        next_node = current->next;
        freeNode(current->info);
        free(current);
        current = next_node;
    }
    free(folder->children);
    free(folder);
}

void freeNode(TreeNode *node)
{
    if (node->type == FILE_NODE)
        freeFile(node->content);
    else
        freeFolder(node->content);
    free(node->name);
    free(node);
}

void freeTree(FileTree *fileTree)
{
    freeNode(fileTree->root);
    free(fileTree);
}

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
        ls_print_folder(currentNode);
    } else {
        List *list = ((FolderContent *)currentNode->content)->children;
        ListNode *node = list->head;
        while (node) {
            if (strcmp(node->info->name, arg) == 0) {
                currentNode = node->info;
                break;
            }
            node = node->next;
        }
        if (!node) {
            printf("ls: cannot access '%s': No such file or directory", arg);
        } else {
            if (currentNode->type == FOLDER_NODE) {
                ls_print_folder(currentNode);
            } else {
                printf("%s: %s\n", arg,
                    ((FileContent *)currentNode->content)->text);
            }
        }
    }
}


void pwd(TreeNode* treeNode)
{
    if (!treeNode->parent) {
        printf("%s", treeNode->name);
        return;
    }

    pwd(treeNode->parent);
    printf("/%s", treeNode->name);
}


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

TreeNode* searchDir(TreeNode* currentNode, char* path)
{
    char *pathCpy = strdup(path);
    TreeNode *node = currentNode;
    if (strcmp(pathCpy, "..") == 0) {
        if (currentNode->parent) {
            free(pathCpy);
            return currentNode->parent;
        } else {
            free(pathCpy);
            return NULL;
        }
    }
    char *token = strtok(pathCpy, "/\n\0");
    while (token) {
        if (strcmp(token, "..") == 0) {
            node = node->parent;
        } else {
            node = search_node(node, token);
            if (!node || node->type == FILE_NODE) {
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
    TreeNode *node = searchDir(currentNode, path);
    if (!node) {
        printf("cd: no such file or directory: %s", path);
        return currentNode;
    }
    return node;
}

void printTree(TreeNode* currentNode, int tabs, int *dirs, int *files)
{
    for (int i = 0; i < tabs; i++)
        printf("\t");
    printf("%s\n", currentNode->name);
    if (currentNode->type == FILE_NODE) {
        (*files)++;
        return;
    }
    (*dirs)++;
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *node = list->head;
    while (node) {
        printTree(node->info, tabs + 1, dirs, files);
        node = node->next;
    }
}

void tree(TreeNode* currentNode, char* arg)
{
    TreeNode *folder = searchDir(currentNode, arg);
    if (!folder) {
        printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
        return;
    }

    int dirs = 0, files = 0;
    List *list = ((FolderContent *)folder->content)->children;
    ListNode *node = list->head;
    while (node) {
        printTree(node->info, 0, &dirs, &files);
        node = node->next;
    }
    printf("%d directories, %d files", dirs, files);
}

TreeNode *emptyDir(char* folderName)
{
    TreeNode *newDir = malloc(sizeof(TreeNode));
    DIE(!newDir, "malloc failed createFileTree: root\n");
    newDir->parent = NULL;
    newDir->name = folderName;
    newDir->type = FOLDER_NODE;
    FolderContent *dirContent = malloc(sizeof(FolderContent));
    List* list = malloc(sizeof(List));
    dirContent->children = list;
    list->head = NULL;
    newDir->content = dirContent;

    return newDir;
}

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
    if (!checkFile(currentNode, folderName)) {
        printf("mkdir: cannot create directory '%s': File exists", folderName);
        free(folderName);
        return;
    }
    TreeNode *newDir = emptyDir(folderName);
    newDir->parent = currentNode;
    addChild(currentNode, newDir);
}


void rmrec(TreeNode* currentNode, char* resourceName)
{
    // TODO
}


void rm(TreeNode* currentNode, char* fileName)
{
    // TODO
}


void rmdir(TreeNode* currentNode, char* folderName)
{
    // TODO
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


void cp(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

void mv(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

