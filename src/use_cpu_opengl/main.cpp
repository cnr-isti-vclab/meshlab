/*
 * This file is part of MeshLab: https://github.com/cnr-isti-vclab/meshlab
 *
 * @author Alessandro Muntoni (muntoni.alessandro@gmail.com)
 */

#ifdef _WIN32
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <windows.h>

bool renameOrCopy();

#ifdef UNICODE
#define MESHLAB_EXE L"meshlab.exe"
#else
#define MESHLAB_EXE "meshlab.exe"
#endif //UNICODE

#endif //_WIN32

int main(int argc, char* argv[])
{
    bool res = false;
    #ifdef _WIN32
    if (argc > 1) {
        res = renameOrCopy();
        if (res){
            //need to start a new meshlab process, using the beautiful call
            //CreateProcess of windows...
            PROCESS_INFORMATION pif;
            STARTUPINFO si;

            ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
            si.cb = sizeof(si);         //Must set size of structure
            CreateProcess(MESHLAB_EXE, NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pif);
        }
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
                               "To undo this, just rename the 'opengl32.dll' file inside the MeshLab folder "
                               "to 'opengl32sw.dll'.",
                               QMessageBox::Yes | QMessageBox::No);

        int button = messageBox.exec();
        if (button == QMessageBox::Yes){
            res = renameOrCopy();

            if (!res){
                QMessageBox messageBox(QMessageBox::Critical, "Error", "Error while renaming/copying opengl32.dll");
                messageBox.exec();
                return -1;
            }
            else {
                QMessageBox messageBox(QMessageBox::Information, "Ok", "opengl32.dll renamed/copied correctly");
                messageBox.exec();
                return 0;
            }
        }
        else {
            return 0;
        }

        return app.exec();
    }
    #else
    std::cerr << "This program is just for windows systems.\n";
    #endif //_WIN32
    return res ? 0 : -1;
}

#ifdef _WIN32
bool renameOrCopy() {
    bool res;

    if (QFile::exists("opengl32sw.dll")){
        res = QFile::rename("opengl32sw.dll", "opengl32.dll");
    }
    else {
        res = QFile::copy(":/dll/opengl32.dll", "opengl32.dll");
    }

    return res;
}
#endif //_WIN32
