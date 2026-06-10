#include <raylib.h>
int main() {
    Image img = LoadImage("about/about.png");
    ExportImageAsCode(img, "about_logo.h");
    UnloadImage(img);
    return 0;
}
