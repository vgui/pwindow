#include <exception>
#include <iostream>
#include "MainWindow.hpp"


int main()
{    
    try
    {
        pwindow::MainWindow window("Main Window", 300, 200, 800, 600);

        return window.Run();
    }
    catch(const std::exception& e)
    {
        std::cout << "Some std::exception was caught." << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "Unknown Error in vgui application !" << std::endl;
    }

    return 0;
}
