#include "Graphics.h"
#include "GUI.h"
#include "Program.h"

int main(void)
{
	Program program;
	program.Init();
	program.Update();
	program.End();
    return 0;
}
