/*
 * This file is part of MeshLab: https://github.com/cnr-isti-vclab/meshlab
 *
 * @author Alessandro Muntoni (muntoni.alessandro@gmail.com)
 */

#include <QFile>
#include <QApplication>
#include <QMessageBox>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

bool renameOrCopy();

int main(int argc, char* argv[])
{
    bool res = false;
    if (argc > 1){
        res = renameOrCopy();
        #ifdef _WIN32
        if (res){
            //need to start a new meshlab process, using the beautiful call
            //CreateProcess of windows...
            PROCESS_INFORMATION pif;  //Gives info on the thread and..
                                       //..process for the new process
            STARTUPINFO si;          //Defines how to start the program

            ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
            si.cb = sizeof(si);         //Must set size of structure
            CreateProcess(
                                L"meshlab.exe", //Path to executable file
                                NULL,   //Command string - not needed here
                                NULL,   //Process handle not inherited
                                NULL,   //Thread handle not inherited
                                false,  //No inheritance of handles
                                0,      //No special flags
                                NULL,   //Same environment block as this prog
                                NULL,   //Current directory - no separate path
                                &si,    //Pointer to STARTUPINFO
                                &pif);   //Pointer to PROCESS_INFORMATION
        }
        #endif //_WIN32
    }
    else {
        QApplication app(argc, argv);

        QMessageBox messageBox(QMessageBox::Question,
                               "Are you sure?",
                               "This program overrides the OpenGL library used by MeshLab with a "
                               "library that runs on CPU, which may lead to very slow graphic performances.\n\n"
                               "This is necessary only if your graphic driver does not support new OpenGL "
                               "versions (and MeshLab does not start).\n\n"
                               "Before clicking YES, be sure that your graphic card drivers are up to date. "
                               "To undo this, just delete the 'opengl32.dll' file inside the MeshLab folder.",
                               QMessageBox::Yes | QMessageBox::No);

        int button = messageBox.exec();
        if (button == QMessageBox::Yes){
            res = renameOrCopy();

            if (!res){
                QMessageBox messageBox(QMessageBox::Critical, "Error", "Error while copying opengl32.dll");
                messageBox.exec();
                return -1;
            }
            else {
                QMessageBox messageBox(QMessageBox::Information, "Ok", "opengl32.dll copied correctly");
                messageBox.exec();
                return 0;
            }
        }
        else {
            return 0;
        }

        return app.exec();
    }
    return res ? 0 : -1;
}

bool renameOrCopy() {
    bool res;

    if (QFile::exists("opengl32sw.dll")){
        QFile::rename("opengl32sw.dll", "opengl32.dll");
    }
    else {
        res = QFile::copy(":/dll/opengl32.dll", "opengl32.dll");
    }

    return res;
}
