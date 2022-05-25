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
    FileTree *new_ft=malloc(sizeof(FileTree));
    DIE(!new_ft, "malloc failed createFileTree: new_ft\n");
    TreeNode *root = malloc(sizeof(TreeNode));
    DIE(!root, "malloc failed createFileTree: root\n");
    root->parent = NULL;
    root->name = rootFolderName;
    root->type = FOLDER_NODE;
    FolderContent *root_content = malloc(sizeof(FolderContent));
    List* list = malloc(sizeof(List));
    root_content->children = list;
    list->head = NULL;

    new_ft->root = root;
    new_ft->root->content = root_content;

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
            printf("ls: cannot access '<%s>': No such file or directory", arg);
        } else {
            if (currentNode->type == FOLDER_NODE) {
                ls_print_folder(currentNode);
            } else {
                printf("a: %s\n", ((FileContent *)currentNode->content)->text);
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

TreeNode* cd(TreeNode* currentNode, char* path)
{
    TreeNode *node = currentNode;
    if (strcmp(path, "..") == 0) {
        if (currentNode->parent)
            return currentNode->parent;
    }
    char *token = strtok(path, "/\0");
    while (token) {
        if (strcmp(path, "..") == 0) {
            currentNode = currentNode->parent;
        } else {
            node = search_node(node, token);
            if (!node) {
                printf("cd: no such file or directory: <%s>", path);
                return currentNode;
            }
        }
        token = strtok(NULL, "/\0");
    }
    return node;
}


void tree(TreeNode* currentNode, char* arg)
{
    // TODO
}


void mkdir(TreeNode* currentNode, char* folderName)
{
    // TODO
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

void addLast(List *list, TreeNode *treeNode)
{
    ListNode *newNode = malloc(sizeof(ListNode));
    DIE(!newNode, "malloc failed addLast: newNode\n");
    newNode->info = treeNode;
    newNode->next = NULL;
    if (!list->head) {
        list->head = newNode;
        return;
    }
    ListNode *current = list->head;
    while (!current->next)
        current = current->next;
    current->next = newNode;
}

void touch(TreeNode* currentNode, char* fileName, char* fileText)
{
    TreeNode *newFile = malloc(sizeof(TreeNode));
    DIE(!newFile, "malloc failed touch: newFile\n");
    newFile->parent = currentNode;
    newFile->name = strdup(fileName);
    newFile->type = FILE_NODE;
    FileContent *fileContent = malloc(sizeof(FileContent));
    DIE(!fileContent, "malloc failed touc: fileContent\n");
    if (fileContent)
        fileContent->text = strdup(fileText);
    else
        fileContent->text = NULL;
    newFile->content = fileContent;
    addLast(((FolderContent *)currentNode->content)->children, newFile);
}


void cp(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

void mv(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

