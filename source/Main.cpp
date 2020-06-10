#include <MainWindow.hpp>


int main(int, char**){
    if(!MainWindow::Initialize()){
        return -1;
    }
    MainWindow::MainLoop();
    MainWindow::Terminate();
    return 0;
}

