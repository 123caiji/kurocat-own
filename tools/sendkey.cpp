#include <windows.h>
#include <cstdio>
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 3;
    for (int i = 0; i < n; i++) {
        INPUT in[2] = {};
        in[0].type = INPUT_KEYBOARD; in[0].ki.wVk = 'A';
        in[1].type = INPUT_KEYBOARD; in[1].ki.wVk = 'A'; in[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &in[0], sizeof(INPUT));
        Sleep(90);
        SendInput(1, &in[1], sizeof(INPUT));
        Sleep(350);
    }
    printf("sent %d presses\n", n);
    return 0;
}
