//
//  main.cpp
//  filesystem
//
//  Created by yjy on 2018/1/4.
//  Copyright © 2018年 yjy. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "operations.h"

//总操作
void operation(string cmd){
    string commands[10];//稍微开大点，防止瞎打空格，0是操作，1、2是路径
    parseCmd(cmd, commands);
    if (commands[0] == "pwd"){
        getLocation();
    }
    else if (commands[0] == "cd"){
        setLocation(commands[1]);
    }
    else if (commands[0] == "mkdir"){
        newItem(commands[1]);
    }
    else if (commands[0] == "ls"){
        getChildItem(commands[1]);
    }
    else if (commands[0] == "rmdir"){
        removeDirectory(commands[1]);
    }
    else if (commands[0] == "echo"){
        writeOutput(commands[1], commands[2]);
    }
    else if (commands[0] == "cat"){
        getContent(commands[1]);
    }
    else if (commands[0] == "rm"){
        removeFile(commands[1]);
    }
    else if (commands[0] == "tree"){
        treeView(commands[1]);
    }
    else if (commands[0] == "cpy"){
        copyFile(commands[1], commands[2]);
    }
    else if (commands[0] == "exit"){
        flag = false;
        cout << "logout" << endl;
    }
    else {
        setColor(COLOR_ERR);
        cout  << cmd << ": command not found" << endl;
        setColor(COLOR_ORIGIN);
    }
    writeMemoryToFile();
}

int main(){
    ifstream fin(FILEPATH, ios::binary);
    if (fin){
        fin.read((char *)&super_block, sizeof(Super_Block));
        fin.read((char *)inodes, sizeof(inodes));
        fin.read((char *)emptyspaces, sizeof(emptyspaces));
    }
    else{
        initialize();
    }
    fin.close();
    while(1){
        string cmd;
        cout << ">>";
        getline (cin, cmd);
        operation(cmd);
        if(!flag)break;
    }
    return 0;
}

