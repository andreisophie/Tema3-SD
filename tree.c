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
    DIE(!new_ft, "malloc failed createFileTree: new_ft");
    TreeNode *root = malloc(sizeof(TreeNode));
    DIE(!root, "malloc failed createFileTree: root");
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

void freeNode(TreeNode *node)
{

}

void freeTree(FileTree *fileTree)
{
    // TODO
}

void ls_print_folder(TreeNode *currentNode)
{
    List *list = (List *)currentNode->content;
    ListNode *node = list->head;
    while (node) {
        printf("%s\n", node->info->name);
        node = node->next;
    }
}

void ls(TreeNode* currentNode, char* arg)
{
    if (!arg) {
        ls_print_folder(currentNode);
    } else {
        if (cmp("FOLDER_NODE", currentNode->type) == 0) {
            List *list = (List *)currentNode->content;
            ListNode *node = list->head;
            while (node) {
                if (cmp(node->info->name, arg) == 0) {
                    currentNode = node->info;
                    break;
                }
                node = node->next;
            }
            ls_print_folder(currentNode);
        } else {
            printf("%s\n", ((FileContent *)currentNode->content)->text);
        }
    }
}


void pwd(TreeNode* treeNode)
{
    // TODO
}


TreeNode* cd(TreeNode* currentNode, char* path)
{
    // TODO
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


void touch(TreeNode* currentNode, char* fileName, char* fileContent)
{
    // TODO

}


void cp(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

void mv(TreeNode* currentNode, char* source, char* destination)
{
    // TODO
}

